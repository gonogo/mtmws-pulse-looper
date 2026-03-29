#include "ComputerCard.h"

class PulseLooper : public ComputerCard {
public:

    // ===== CONFIG =====
    static const int MAX_EVENTS = 4096;

    struct Event {
        uint32_t dt;
    };

    Event events[MAX_EVENTS];
    uint32_t event_count = 0;

    // ===== STATE =====
    bool recording = false;
    bool playing = false;

    uint32_t last_time = 0;
    uint32_t play_time = 0;
    uint32_t play_index = 0;

    uint32_t accumulator = 0;

    uint32_t sample_count = 0;

    // ===== PARAMETERS =====
    float speed = 1.0f;
    float probability = 1.0f;
    uint32_t pulse_width = 1000;

    uint32_t pulse_timer = 0;

    // ===== INIT =====
    void Init() {
        event_count = 0;
        sample_count = 0;
    }

    // ===== RANDOM =====
    float RandomFloat() {
        static uint32_t lcg_seed = 1;
        lcg_seed = 1664525 * lcg_seed + 1013904223;
        return (lcg_seed >> 16) / 65536.0f;
    }

    // ===== TIME =====
    uint32_t Time() {
        return sample_count * 21; // approx microseconds at 48kHz
    }

    // ===== SAMPLE TIME =====
    uint32_t GetSampleTime() {
        return 21; // microseconds per sample
    }

    // ===== MAIN LOOP =====
    void ProcessSample() override {

        sample_count++;

        // --- Read controls ---
        speed = 0.5f + KnobVal(Knob::Main) * 1.5f / 4095.0f;
        probability = KnobVal(Knob::X) / 4095.0f;
        pulse_width = 500 + KnobVal(Knob::Y) * 5000 / 4095;

        bool pulse_in = PulseIn1();
        bool reset_in = PulseIn2();
        bool rec_in = SwitchVal() == Switch::Up; // assuming switch for record

        uint32_t now = Time(); // microseconds (approximate)

        // --- RECORD LOGIC ---
        if (rec_in && !recording) {
            recording = true;
            LedOn(4);
            event_count = 0;
        }

        if (!rec_in && recording) {
            recording = false;
            LedOff(4);
            LedOff(1);
            playing = true;
            play_index = 0;
            accumulator = 0;
        }

        if (recording && pulse_in) {
            if (event_count == 0) {
                last_time = now;
            } else {
                uint32_t dt = now - last_time;

                if (dt > 200 && event_count < MAX_EVENTS) {
                    events[event_count++].dt = dt;
                    LedOn(1);
                }

                last_time = now;
            }
        }

        // --- RESET ---
        if (reset_in) {
            play_index = 0;
            accumulator = 0;
        }

        // --- PLAYBACK ---
        if (playing && event_count > 0) {

            accumulator += (uint32_t)(GetSampleTime() * speed);

            if (accumulator >= events[play_index].dt) {

                accumulator = 0;

                // probability
                if (RandomFloat() < probability) {
                    PulseOut1(true);
                    pulse_timer = pulse_width;
                }

                play_index++;
                if (play_index >= event_count) play_index = 0;
            }
        }

        // --- Pulse OFF timing ---
        if (pulse_timer > 0) {
            pulse_timer -= GetSampleTime();
            if (pulse_timer <= 0) {
                PulseOut1(false);
            }
        }
    }
};

// ===== MAIN =====
int main()
{
    PulseLooper looper;
    looper.Init();
    looper.Run();
}