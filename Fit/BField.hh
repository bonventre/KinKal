#ifndef KinKal_BField_hh
#define KinKal_BField_hh
//
// Effect to correct the fit parameters for the change in BField along a small piece of the trajectory.
// This effect adds no information content or noise (presently), just transports the parameters
//
#include "KinKal/Fit/Effect.hh"
#include "KinKal/General/TimeDir.hh"
#include "KinKal/General/BFieldMap.hh"
#include "KinKal/Fit/Config.hh"
#include <iostream>
#include <stdexcept>
#include <array>
#include <ostream>

namespace KinKal {
  template<class KTRAJ> class BField : public Effect<KTRAJ> {
    public:
      using KKEFF = Effect<KTRAJ>;
      using PKTRAJ = ParticleTrajectory<KTRAJ>;

      double time() const override { return drange_.mid(); } // apply the correction at the middle of the range
      bool active() const override { return bfcorr_; }
      void process(FitState& kkdata,TimeDir tdir) override;
      void update(PKTRAJ const& ref) override;
      void update(PKTRAJ const& ref, MetaIterConfig const& miconfig) override;
      void update(Config const& config) override { bfcorr_ = config.bfcorr_; }
      void print(std::ostream& ost=std::cout,int detail=0) const override;
      void append(PKTRAJ& fit) override;
      Chisq chisq(Parameters const& pdata) const override { return Chisq();}
      Parameters const& effect() const { return dbforw_; }
      virtual ~BField(){}
      // disallow copy and equivalence
      BField(BField const& ) = delete;
      BField& operator =(BField const& ) = delete;
      // create from the domain range, the effect, and the
      BField(Config const& config, BFieldMap const& bfield,TimeRange const& drange) :
        bfield_(bfield), drange_(drange), bfcorr_(config.bfcorr_) {}
      TimeRange const& range() const { return drange_; }

    private:
      BFieldMap const& bfield_; // bfield
      TimeRange drange_; // extent of this effect.  The middle is at the transition point between 2 bfield domains (domain transition)
      Parameters dbforw_; // aggregate effect in parameter space of BFieldMap change in the forwards direction
      bool bfcorr_; // apply correction or not
  };

  template<class KTRAJ> void BField<KTRAJ>::process(FitState& kkdata,TimeDir tdir) {
    if(bfcorr_){
      kkdata.append(dbforw_,tdir);
    }
  }

  template<class KTRAJ> void BField<KTRAJ>::update(PKTRAJ const& ref) {
  }

  template<class KTRAJ> void BField<KTRAJ>::update(PKTRAJ const& ref, MetaIterConfig const& miconfig) {
    update(ref);
  }

  template<class KTRAJ> void BField<KTRAJ>::append(PKTRAJ& pktraj) {
    if(bfcorr_){
      double etime = time();
      // make sure the piece is appendable
      if(pktraj.back().range().begin() > etime) throw std::invalid_argument("BField: Can't append piece");
      TimeRange newrange(etime,std::max(pktraj.range().end(),drange_.end()));
      // copy the back piece of pktraj and set its range
      KTRAJ newpiece(pktraj.back());
      newpiece.range() = newrange;
      // update the parameters according to the change in bnom across this domain
      VEC3 newbnom = bfield_.fieldVect(pktraj.position3(drange_.end()));
      newpiece.setBNom(etime,newbnom);
      pktraj.append(newpiece);
      //
      auto const& begtraj = pktraj.nearestPiece(drange_.begin());
      auto const& endtraj = pktraj.nearestPiece(drange_.end());
      dbforw_.parameters() = begtraj.dPardB(etime,endtraj.bnom());
    }
  }

  template<class KTRAJ> void BField<KTRAJ>::print(std::ostream& ost,int detail) const {
    ost << "BField " << static_cast<Effect<KTRAJ>const&>(*this);
    ost << " effect " << dbforw_.parameters() << " domain range " << drange_ << std::endl;
  }

  template <class KTRAJ> std::ostream& operator <<(std::ostream& ost, BField<KTRAJ> const& kkmat) {
    kkmat.print(ost,0);
    return ost;
  }

}
#endif
