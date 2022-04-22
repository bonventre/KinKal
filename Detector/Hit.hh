#ifndef KinKal_Hit_hh
#define KinKal_Hit_hh
//
//  Base class to describe a measurement that constrains some aspect of the track fit
//  The constraint is expressed as a weight WRT a set of reference parameters.
//  The base class is a purely algebraic object.
//
#include "KinKal/General/Weights.hh"
#include "KinKal/General/Parameters.hh"
#include "KinKal/General/Chisq.hh"
#include "KinKal/Trajectory/ParticleTrajectory.hh"
#include "KinKal/Fit/MetaIterConfig.hh"
#include <memory>
#include <ostream>

namespace KinKal {
  template <class KTRAJ> class Hit {
    public:
      using PKTRAJ = ParticleTrajectory<KTRAJ>;
      using KTRAJPTR = std::shared_ptr<KTRAJ>;
      Hit() : wscale_(1.0){}
      virtual ~Hit(){}
      // disallow copy and equivalence
      Hit(Hit const& ) = delete;
      Hit& operator =(Hit const& ) = delete;
     // hits may be active (used in the fit) or inactive; this is a pattern recognition feature
      virtual bool active() const =0;
      virtual Chisq chisq(Parameters const& params) const =0;  // least-squares distance to given parameters
      virtual double time() const = 0;  // time of this hit: this is WRT the reference trajectory
      // update to a new reference, without changing internal state
      virtual void update(PKTRAJ const& pktraj) = 0;
      // update the internals of the hit, specific to this meta-iteraion
      virtual void update(PKTRAJ const& pktraj,MetaIterConfig const& config) = 0;
      // update the weight
      virtual void updateWeight() = 0;
      virtual void print(std::ostream& ost=std::cout,int detail=0) const = 0;
      // accessors
      // the constraint this hit implies WRT the current trajectory, expressed as a weight
      Weights const& weight() const { return weight_; }
      // the same, scaled for annealing
      double weightScale() const { return wscale_; }
      KTRAJ const& referenceTrajectory() const { return *reftraj_; }  // trajectory WRT which the weight etc is defined
      // parameters WRT which this hit's residual and weights are set.  These are generally biased
      // in that they contain the information of this hit
      Parameters const& referenceParameters() const { return referenceTrajectory().params(); }
      // Unbiased parameters, taking out the hits effect
      Parameters unbiasedParameters() const;
      // unbiased least-squares distance to reference parameters
      Chisq chisquared() const;
      // update the weight
      void setWeight(Weights const& weight){
        weight_ = weight;
        weight_ *= wscale_;
      }
      void setWeightScale(double wscale) {
        wscale_ = wscale;
      }
      void setRefTraj(KTRAJPTR const& reftraj) { reftraj_ = reftraj; }
    private:
      double wscale_; // current annealing weight scaling
      Weights weight_; // weight representation of the hit's constraint
      KTRAJPTR reftraj_; // reference WRT this hits weight was calculated
  };

  template<class KTRAJ> Parameters Hit<KTRAJ>::unbiasedParameters() const {
    if(active()){
      // convert the parameters to a weight, and subtract this hit's weight
      Weights wt(referenceParameters());
      // subtract out the effect of this hit's reference weight from the reference parameters
      wt -= weight_;
      return Parameters(wt);
    } else
      return referenceParameters();
  }

  template<class KTRAJ> Chisq Hit<KTRAJ>::chisquared() const {
    if(active()){
      return chisq(unbiasedParameters());
    } else
      return Chisq();
  }

  template <class KTRAJ> std::ostream& operator <<(std::ostream& ost, Hit<KTRAJ> const& thit) {
    thit.print(ost,0);
    return ost;
  }

}
#endif

