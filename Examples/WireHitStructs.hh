#ifndef KinKal_WireHitStructs_hh
#define KinKal_WireHitStructs_hh
#include <stdexcept>
namespace KinKal {
// struct describing local drift info
  struct DriftInfo {
    DriftInfo() : tdrift_(0.0), tdriftvar_(0.0), vdrift_(0.0) {}
    double tdrift_; // drift time
    double tdriftvar_; // variance on drift time
    double vdrift_; // instantanious drift speed
  };

  // struct describing wire hit internal state
  struct WireHitState {
    enum State { inactive=-2, left=-1, null=0, right=1};  // state description
    enum LState { longinactive=0, longactive=1}; // use longitudinal measurement 
    State state_; // left-right ambiguity
    LState lstate_; // time division
    bool useDrift() const { return state_ == left || state_ == right; }
    bool useLong() const { return lstate_ == longactive; }
    bool active() const { return state_ != inactive; }
    bool operator == (WireHitState::State state) const { return state_ == state; }
    bool operator != (WireHitState::State state) const { return state_ != state; }
    bool operator == (WireHitState::LState state) const { return lstate_ == state; }
    bool operator != (WireHitState::LState state) const { return lstate_ != state; }
    double lrSign() const {
      switch (state_) {
        case left:
          return -1.0;
        case right:
          return 1.0;
        default:
          return 0.0;
      }
    }
    WireHitState(State state = inactive, LState lstate = longinactive) : state_(state), lstate_(lstate) {}
  };
}
#endif
