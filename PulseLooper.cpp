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
    
    uint32_t playHead1 = 0;
    uint32_t playHead2 = 0;

    uint32_t sample_count = 0;

    // ===== PARAMETERS =====
    // speed_fp: Q16.16 fixed-point (1.0 = 65536)
    uint32_t speed_fp = 65536;
    // probability_fp: Q0.16 fixed-point (1.0 = 65535)
    uint32_t probability_fp = 65535;
    uint32_t pulse_width = 1000;

    uint32_t pulse_timer = 0;

    // ===== INIT =====
    void Init() {
        event_count = 0;
        sample_count = 0;
    }

    // ===== RANDOM =====
    uint32_t RandomFixed16() {
        static uint32_t lcg_seed = 1;
        lcg_seed = 1664525 * lcg_seed + 1013904223;
        return (lcg_seed >> 16) & 0xFFFF; // 0..65535
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
        // speed_fp: 0.5..2.0 => Q16.16
        uint32_t main_raw = KnobVal(Knob::Main);
        speed_fp = 32768 + (main_raw * 98304 + 2047) / 4095;

        // probability_fp: 0..1 => Q0.16
        uint32_t x_raw = KnobVal(Knob::X);
        probability_fp = (x_raw * 65535 + 2047) / 4095;

        pulse_width = 500 + KnobVal(Knob::Y) * 5000 / 4095;

        bool pulse_in = PulseIn1();
        bool pulse_off = PulseIn1FallingEdge();
        bool reset_in = PulseIn2();
        bool rec_in = SwitchVal() == Switch::Up; // assuming switch for record

        uint32_t now = Time(); // microseconds (approximate)

        // --- RECORD LOGIC ---
        if (rec_in && !recording) {
            recording = true;
            LedOn(0);
            event_count = 0;
        }

        if (!rec_in && recording) {
            LedOff(0);
            recording = false;
            playing = true;
            play_index = 0;
            accumulator = 0;
        }

        if (recording && pulse_in) {
            LedOn(4);
            if (event_count == 0) {
                last_time = now;
                uint32_t dt = 0;
                events[event_count++].dt = dt;
            } else {
                uint32_t dt = now - last_time;

                if (dt > 200 && event_count < MAX_EVENTS) {
                    events[event_count++].dt = dt;
                }

                last_time = now;
            }
        }
if (recording && pulse_off) {
            LedOff(4);
}

        // --- RESET ---
        if (reset_in) {
            play_index = 0;
            accumulator = 0;
        }

        // --- PLAYBACK ---
        if (playing && event_count > 0) {
        

            accumulator += (uint32_t)(GetSampleTime() * speed_fp);
            if (accumulator >= 200) {
                // If pulse is true, leave it up for a while to make sure it triggers??
                PulseOut1(false);
            }

            if (accumulator >= events[play_index].dt) {

                accumulator = 0;

                // probability
                //if (RandomFloat() < probability) {
                    PulseOut1(true);
                    if (playHead2 == 1) {
                        playHead2 = 0;
                        playHead1 = 1;
                        LedOn(2);
                        LedOff(3);
                    } else {
                        playHead1 = 0;
                        playHead2 = 1;
                        LedOn(3);
                        LedOff(2);
                    }
                //    pulse_timer = pulse_width;
                //}

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