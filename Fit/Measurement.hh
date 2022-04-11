#ifndef KinKal_Measurement_hh
#define KinKal_Measurement_hh
//
//  class represeting a constraint on the fit parameters due external information (typically a measurement).
//  Used as part of the kinematic Kalman fit
//
#include "KinKal/Fit/Effect.hh"
#include "KinKal/Trajectory/ParticleTrajectory.hh"
#include "KinKal/Detector/Hit.hh"
#include <ostream>
#include <memory>

namespace KinKal {
  template <class KTRAJ> class Measurement : public Effect<KTRAJ> {
    public:
      using KKEFF = Effect<KTRAJ>;
      using PKTRAJ = ParticleTrajectory<KTRAJ>;
      using HIT = Hit<KTRAJ>;
      using HITPTR = std::shared_ptr<HIT>;
      // Effect Interface
      double time() const override { return hit_->time(); }
      bool active() const override { return hit_->active(); }
      void process(FitState& kkdata,TimeDir tdir) override;
      void update(PKTRAJ const& pktraj) override;
      void update(PKTRAJ const& pktraj, MetaIterConfig const& miconfig) override;
      void update(Config const& config) override {}
      void append(PKTRAJ& fit) override;
      Chisq chisq(Parameters const& pdata) const override;
      void print(std::ostream& ost=std::cout,int detail=0) const override;
      virtual ~Measurement(){}
      // local functions
      // construct from a hit and reference trajectory
      Measurement(HITPTR const& hit, PKTRAJ const& reftraj);
      // access the underlying hit
      HITPTR const& hit() const { return hit_; }
      Weights const& weight() const { return hit_->weight(); }
    private:
      HITPTR hit_ ; // hit used for this constraint
  };

  template<class KTRAJ> Measurement<KTRAJ>::Measurement(HITPTR const& hit, PKTRAJ const& reftraj) : hit_(hit) {
    update(reftraj);
  }

  template<class KTRAJ> void Measurement<KTRAJ>::process(FitState& kkdata,TimeDir tdir) {
    // direction is irrelevant for processing hits
    if(this->active()){
      // add this effect's information
      kkdata.append(weight());
    }
    KKEFF::setState(tdir,KKEFF::processed);
  }

  template<class KTRAJ> void Measurement<KTRAJ>::update(PKTRAJ const& pktraj) {
    // update the hit
    hit_->update(pktraj);
    // ready for processing!
    KKEFF::updateState();
  }

  template<class KTRAJ> void Measurement<KTRAJ>::update(PKTRAJ const& pktraj, MetaIterConfig const& miconfig) {
    // update the hit's internal state; the actual update depends on the hit
    hit_->update(pktraj,miconfig );
    // ready for processing!
    KKEFF::updateState();
  }

  template<class KTRAJ> void Measurement<KTRAJ>::append(PKTRAJ& fit) {
    // update the hit to this trajectory
//    hit_->update(pktraj);
  }

  template<class KTRAJ> Chisq Measurement<KTRAJ>::chisq(Parameters const& pdata) const {
    return hit_->chisq(pdata);
  }

  template <class KTRAJ> void Measurement<KTRAJ>::print(std::ostream& ost, int detail) const {
    ost << "Measurement " << static_cast<Effect<KTRAJ> const&>(*this) << std::endl;
    if(detail > 0){
      hit_->print(ost,detail);
    }
  }

  template <class KTRAJ> std::ostream& operator <<(std::ostream& ost, Measurement<KTRAJ> const& kkhit) {
    kkhit.print(ost,0);
    return ost;
  }

}
#endif
