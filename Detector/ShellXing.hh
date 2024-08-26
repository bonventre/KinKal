#ifndef KinKal_ShellXing_hh
#define KinKal_ShellXing_hh
//
//  Describe the effects of a kinematic trajectory crossing a thin shell of material defined by a surface
//  Used in the kinematic Kalman fit
//
#include "KinKal/Detector/ElementXing.hh"
#include "KinKal/Geometry/Surface.hh"
#include "KinKal/Geometry/ParticleTrajectoryIntersect.hh"
#include "KinKal/MatEnv/DetMaterial.hh"

namespace KinKal {
  template <class KTRAJ,class SURF> class ShellXing : public ElementXing<KTRAJ> {
    public:
      using PTRAJ = ParticleTrajectory<KTRAJ>;
      using KTRAJPTR = std::shared_ptr<KTRAJ>;
      using EXING = ElementXing<KTRAJ>;
      using PCA = PiecewiseClosestApproach<KTRAJ,SensorLine>;
      using CA = ClosestApproach<KTRAJ,SensorLine>;
      using SURFPTR = std::shared_ptr<SURF>;
      // construct from a surface, material, intersection, and transverse thickness
      ShellXing(SURFPTR surface, MatEnv::DetMaterial const& mat, Intersection inter, double thickness)
      virtual ~ShellXing() {}
      // ElementXing interface
      void updateReference(PTRAJ const& ptraj) override;
      void updateState(MetaIterConfig const& config,bool first) override;
      Parameters params() const override;
      double time() const override { return inter_.time(); }
      KTRAJ const& referenceTrajectory() const override { return tpca_.particleTraj(); }
      std::vector<MaterialXing>const&  matXings() const override { return mxings_; }
      void print(std::ostream& ost=std::cout,int detail=0) const override;
      // accessors
    private:
      SURFPTR surf_; // surface
      MatEnv::DetMaterial const& mat_;
      Intersection inter_; // most recent intersection
      std::vector<MaterialXing> mxings_; // material xing
      double thick_; // shell thickness
      double tol_; // tolerance for intersection
      double varscale_; // variance scale, for annealing
      Parameters fparams_; // 1st-order parameter change for forwards time
  };

  template <class KTRAJ> ShellXing<KTRAJ>::ShellXing(SURFPTR surface, MatEnv::DetMaterial const& mat, Intersection inter, double thickness, double tol) :
    surf_(surface), mat_(mat), inter_(inter),thick_(thick),tol_(tol),
    varscale_(1.0)
  {}

  template <class KTRAJ> void ShellXing<KTRAJ>::updateReference(PTRAJ const& ptraj) {

    // re-intersect with the surface, taking the current time as start and range from the current piece (symmetrized)
    TimeRange irange(
    //
  }

  template <class KTRAJ> void ShellXing<KTRAJ>::updateState(MetaIterConfig const& miconfig,bool first) {
    if(first) {
      // search for an update to the xing configuration among this meta-iteration payload
      auto sxconfig = miconfig.findUpdater<ShellXingConfig>();
      if(sxconfig != 0){
        sxconfig_ = *sxconfig;
      }
      if(sxconfig_.scalevar_)
        varscale_ = miconfig.varianceScale();
      else
        varscale_ = 1.0;
    }
    smat_.findXings(tpca_.tpData(),sxconfig_,mxings_);
    // reset
    fparams_ = Parameters();
    if(mxings_.size() > 0){
    }
  }

  template <class KTRAJ> Parameters ShellXing<KTRAJ>::params() const {
    return fparams_;
  }

  template <class KTRAJ> void ShellXing<KTRAJ>::print(std::ostream& ost,int detail) const {
    ost <<"Shell Xing time " << this->time();
    ost << std::endl;
  }

}
#endif
