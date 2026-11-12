/*  Stream History Session
 *
 *  From: https://github.com/PokemonAutomation/
 *
 */

#include <vector>
#include <QAudioFormat>
#include "Common/Cpp/Exceptions.h"
#include "Common/Cpp/Containers/Pimpl.tpp"   //  Pimpl<QAudioFormat> (AudioFormat) instantiations
#include "Common/Cpp/Concurrency/SpinLock.h"
#include "Common/Cpp/Concurrency/Mutex.h"
#include "CommonFramework/GlobalSettingsPanel.h"
#include "CommonFramework/AudioPipeline/AudioFormat.h"
#include "CommonFramework/AudioPipeline/AudioInputChoice.h"
#include "CommonFramework/VideoPipeline/Backends/VideoFrameQt.h"
#include "CommonFramework/Recording/StreamHistoryOption.h"

#if defined(PA_STREAM_HISTORY_FFMPEG)
#include "StreamHistoryTracker_Ring.h"
using StreamHistoryTracker = PokemonAutomation::StreamHistoryTracker_Ring;
#elif (QT_VERSION_MAJOR == 6) && (QT_VERSION_MINOR >= 8)
#include "StreamHistoryTracker_SaveFrames.h"
//#include "StreamHistoryTracker_RecordOnTheFly.h"
//#include "StreamHistoryTracker_ParallelStreams.h"
#else
#include "StreamHistoryTracker_Null.h"
#endif


#include "StreamHistorySession.h"



namespace PokemonAutomation{





struct StreamHistorySession::Data{
    Logger& m_logger;
    mutable SpinLock m_lock;
    std::chrono::seconds m_window;
    //  The resolved input choice for the active device input (invalid
    //  when there is no device input). With the ffmpeg ring it only
    //  decides whether audio is recorded; otherwise it drives both the
    //  tracker's shape and the working-stream attach shape.
    AudioInputChoice m_audio_choice;
    bool m_has_video;
    //  The active input's physical working-stream shape (what the tree
    //  delivers through the working listener in the non-ffmpeg builds,
    //  where the tracker records at the delivered shape; the ffmpeg ring
    //  records a fixed 48 kHz stereo). Set in initialize().
    size_t m_active_channels = 0;
    size_t m_active_rate = 0;
    std::vector<float> m_interleaved;
    //  The audio session the working listener attaches to (set_audio_session).
    AudioSession* m_audio_session = nullptr;
    std::shared_ptr<StreamHistoryTracker> m_current;
    //  Bumped whenever the tracker settings change or the tracker is
    //  cleared. Whoever bumps it passes the new value to initialize(), which
    //  builds the tracker outside the lock and only installs it if no other
    //  change happened since that bump, so a tracker built from stale
    //  settings never replaces a newer one or undoes a clear().
    uint64_t m_generation = 0;
#if defined(PA_STREAM_HISTORY_FFMPEG)
    //  Held by push_samples() while it pushes into the tracker outside
    //  m_lock (it only contends with retire()).
    Mutex m_push_lock;
#endif

    Data(Logger& logger)
        : m_logger(logger)
        , m_window(GlobalSettings::instance().STREAM_HISTORY->HISTORY_SECONDS)
        , m_has_video(false)
    {}

    //  Destroys a tracker already taken out of m_current (call without
    //  m_lock). With the ffmpeg ring, first waits for an in-flight
    //  push_samples(), which may still be using it: the audio thread must
    //  never be left holding the last reference, because destroying the
    //  tracker waits for its video encoder thread, and the audio pipeline
    //  would stall meanwhile.
    void retire(std::shared_ptr<StreamHistoryTracker>& tracker){
#if defined(PA_STREAM_HISTORY_FFMPEG)
        if (tracker){
            std::lock_guard<Mutex> push_guard(m_push_lock);
        }
#endif
        tracker.reset();
    }
};




StreamHistorySession::~StreamHistorySession() = default;
StreamHistorySession::StreamHistorySession(Logger& logger)
    //  The working-stream nominal shape is metadata only: the pair's
    //  per-listener forwarder carries the shape actually requested, and
    //  nothing validates this one.
    : Kernels::AudioPipeline::WorkingAudioStreamListener(48000, 2)
    , m_data(CONSTRUCT_TOKEN, logger)
{}
void StreamHistorySession::start(
    const AudioInputChoice& choice, bool has_video
){
    Data& data = *m_data;
    bool needs_tracker;
    uint64_t generation = 0;
    {
        WriteSpinLock lg(data.m_lock);
        needs_tracker = !data.m_current;
        if (needs_tracker){
            data.m_audio_choice = choice;
            data.m_has_video = has_video;
            generation = ++data.m_generation;
        }
    }
    if (needs_tracker){
        initialize(generation);
    }
    attach_working_listener();
}
void StreamHistorySession::set_audio_session(AudioSession& session){
    Data& data = *m_data;
    WriteSpinLock lg(data.m_lock);
    data.m_audio_session = &session;
}



bool StreamHistorySession::save(const std::string& filename) const{
    //  This will be coming in from random threads. It will block until the save
    //  is finished or failed.

    const Data& data = *m_data;

    //  Get an owning reference to the current tracker.
    //  This will allow us to promptly release the lock and unblock the UI from
    //  changing the streams (which will wipe the history).
    std::shared_ptr<StreamHistoryTracker> tracker;
    {
        WriteSpinLock lg(data.m_lock);
        if (!data.m_current){
            data.m_logger.log("Cannot save stream history: Stream history is not enabled.", COLOR_RED);
            return false;
        }
        tracker = data.m_current;
    }

//    tracker->save(m_logger, filename);
//    HistorySaverThread saver(*tracker, filename);
//    return saver.save();

    return tracker->save(filename);
}
void StreamHistorySession::push_samples(size_t samples, size_t stride, const float* data){
    Data& state = *m_data;
#if defined(PA_STREAM_HISTORY_FFMPEG)
    //  The ring tracker consumes the planar working stream directly at its
    //  fixed 48 kHz / stereo attach shape; nothing to convert here.
    //  It encodes AAC inline, so it is called with an owning reference
    //  rather than under the lock, which the video thread (on_frame()) and
    //  the UI (save(), clear(), initialize()) spin on. This thread is the
    //  only caller of the tracker's push_samples(): the working-stream
    //  listener is removed (quiescing this call) before it is re-attached.
    //  The reference is released before m_push_lock (declared first), so
    //  a tracker cleared meanwhile is destroyed by retire(), not here.
    if (samples == 0){
        return;
    }
    std::lock_guard<Mutex> push_guard(state.m_push_lock);
    std::shared_ptr<StreamHistoryTracker> tracker;
    {
        WriteSpinLock lg(state.m_lock);
        tracker = state.m_current;
    }
    if (tracker){
        tracker->push_samples(samples, stride, data);
    }
#else
    WriteSpinLock lg(state.m_lock);
    if (!state.m_current || samples == 0){
        return;
    }
    //  The working stream is split (planar) channels: channel "c" starts at
    //  "data[c * stride]" and runs for "samples" samples. (stride may be 0
    //  for the single-channel pass-through.) Re-interleave for the tracker.
    //  The capture card's RL channel swap (INTERLEAVE_RL_96000) is already
    //  applied by the pair's conversion node matrix, the same way as for
    //  the sink branch.
    if (state.m_active_channels == 0){
        return;
    }
    const size_t channels = state.m_active_channels;
    state.m_interleaved.resize(samples * channels);
    for (size_t i = 0; i < samples; i++){
        for (size_t c = 0; c < channels; c++){
            state.m_interleaved[i * channels + c] = data[c * stride + i];
        }
    }
    state.m_current->on_samples(state.m_interleaved.data(), samples);
#endif
}
void StreamHistorySession::on_frame(std::shared_ptr<const VideoFrame> frame){
    Data& data = *m_data;
    WriteSpinLock lg(data.m_lock);
    if (data.m_current){
        data.m_current->on_frame(std::move(frame));
    }
}


void StreamHistorySession::clear(){
//    cout << "clear()" << endl;
    Data& data = *m_data;
    data.m_logger.log("Clearing stream history...", COLOR_ORANGE);

    std::shared_ptr<StreamHistoryTracker> old_tracker;
    {
        WriteSpinLock lg(data.m_lock);
        old_tracker = std::move(data.m_current);
        data.m_generation++;
    }
    data.retire(old_tracker);
}
void StreamHistorySession::initialize(uint64_t generation){
    if (!GlobalSettings::instance().STREAM_HISTORY->enabled()){
        return;
    }

    Data& data = *m_data;
    data.m_logger.log("Starting stream history...", COLOR_ORANGE);

    AudioInputChoice choice;
    bool has_video;
    std::chrono::seconds window;
    {
        WriteSpinLock lg(data.m_lock);
        if (generation != data.m_generation){
            return;    //  Already superseded (or cleared): don't build it.
        }
        choice = data.m_audio_choice;
        has_video = data.m_has_video;
        window = data.m_window;
    }

//    cout << "video = " << has_video << endl;

    //  The tracker's audio shape is the input source's working-stream
    //  shape (what the tree delivers through the working listener), i.e.
    //  the resolved input choice's logical shape: the preset's framing,
    //  with the capture-card 96 kHz reinterpretation applied for the
    //  reinterpreted choices. No choice (file input or no input) records
    //  at no shape.
#if defined(PA_STREAM_HISTORY_FFMPEG)
    //  The ring ignores the shape entirely: it records at a fixed 48 kHz /
    //  stereo (see attach_working_listener()), so the choice only gates
    //  whether audio is captured at all.
    std::shared_ptr<StreamHistoryTracker> tracker = std::make_shared<StreamHistoryTracker>(
        data.m_logger, window, static_cast<bool>(choice), has_video
    );
#else
    const size_t channels = choice ? choice.format().channels() : 0;
    const size_t rate = choice ? choice.format().sample_rate() : 0;
    std::shared_ptr<StreamHistoryTracker> tracker = std::make_shared<StreamHistoryTracker>(
        data.m_logger, window, channels, rate, has_video
    );
#endif

    //  Install it, unless the settings changed while it was being built
    //  (then the newer initialize() installs its own, or the history was
    //  cleared). Whichever tracker loses is destroyed after the lock is
    //  released.
    std::shared_ptr<StreamHistoryTracker> old_tracker;
    {
        WriteSpinLock lg(data.m_lock);
        if (generation != data.m_generation){
            old_tracker = std::move(tracker);
        }else{
            old_tracker = std::move(data.m_current);
            data.m_current = std::move(tracker);
#if !defined(PA_STREAM_HISTORY_FFMPEG)
            data.m_active_channels = channels;
            data.m_active_rate = rate;
#endif
        }
    }
    data.retire(old_tracker);
}
void StreamHistorySession::pre_input_change(){
//    cout << "pre_input_change()" << endl;
    clear();
}
void StreamHistorySession::post_input_change(
    const std::shared_ptr<const AudioSourceDescriptor>& source,
    const AudioInputChoice& choice
){
//    cout << "post_input_change()" << endl;
    Data& data = *m_data;
    AudioSession* audio_session;
    {
        WriteSpinLock lg(data.m_lock);
        audio_session = data.m_audio_session;
    }
    //  Detach the working listener first: after this returns, no pushes in
    //  the old shape can reach push_samples() (the pair quiesces in-flight
    //  pushes). Then update the shapes, then attach for the new shape.
    if (audio_session){
        audio_session->remove_working_stream_listener(*this);
    }
    uint64_t generation;
    {
        WriteSpinLock lg(data.m_lock);
        data.m_audio_choice = choice;
        generation = ++data.m_generation;
    }
    initialize(generation);
    attach_working_listener();
}
void StreamHistorySession::attach_working_listener(){
    Data& data = *m_data;
    AudioSession* audio_session;
    AudioInputChoice target;
    {
        WriteSpinLock lg(data.m_lock);
        audio_session = data.m_audio_session;
        target = data.m_audio_choice;
    }
    if (!audio_session || !target){
        return;
    }
    //  The remove quiesces any in-flight pushes in the old shape (and is a
    //  no-op if not attached yet); the node is then built for the new one.
    audio_session->remove_working_stream_listener(*this);
#if defined(PA_STREAM_HISTORY_FFMPEG)
    //  The ring tracker encodes AAC at 48 kHz stereo: request that fixed
    //  shape and the pair's per-listener node mixes + resamples every input
    //  preset into it (mono upmixes to dual-mono). The shape never changes
    //  with the input, so on input changes this just rebuilds the same node
    //  (this session remains the listener; it forwards to whichever tracker
    //  is current).
    QAudioFormat target_format;
    target_format.setSampleRate(48000);
    target_format.setChannelCount(2);
    target_format.setChannelConfig(QAudioFormat::ChannelConfigStereo);
    audio_session->add_working_stream_listener(AudioFormat(target_format), *this);
#else
    audio_session->add_working_stream_listener(
        AudioFormat(target.format().native_format()), *this
    );
#endif
}
void StreamHistorySession::pre_shutdown(){
//    cout << "pre_shutdown()" << endl;
    clear();
}
void StreamHistorySession::post_shutdown(){
//    cout << "post_shutdown()" << endl;
    Data& data = *m_data;
    uint64_t generation;
    {
        WriteSpinLock lg(data.m_lock);
        data.m_has_video = false;
        generation = ++data.m_generation;
    }
    initialize(generation);
}
void StreamHistorySession::post_startup(VideoSource* source){
    Data& data = *m_data;
    uint64_t generation;
    {
        WriteSpinLock lg(data.m_lock);
        data.m_has_video = source != nullptr;
        generation = ++data.m_generation;
    }
    initialize(generation);
}
void StreamHistorySession::pre_resolution_change(Resolution resolution){
//    cout << "pre_resolution_change()" << endl;
    clear();
}
void StreamHistorySession::post_resolution_change(Resolution resolution){
//    cout << "post_resolution_change()" << endl;
    Data& data = *m_data;
    uint64_t generation;
    {
        WriteSpinLock lg(data.m_lock);
        generation = ++data.m_generation;
    }
    initialize(generation);
}







}
