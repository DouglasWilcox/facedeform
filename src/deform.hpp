#pragma once
#include <memory>
#include <unordered_map>
#include <SOP/SOP_Node.h>
#include "alglib_rbf.hpp"

namespace facedeform {

#ifndef NDEBUG
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) do {} while (0)
#endif

template<class Geometry_Type> 
class DeformerBase
{   
public:
    virtual bool init(const Geometry_Type *, const Geometry_Type *, 
        const Geometry_Type *)               = 0;
    virtual bool is_init()  const            = 0;
    // See above
    virtual bool build(const float & radius) = 0;
    virtual bool is_built() const            = 0;
    virtual void * interpolant() const       = 0;
    virtual void deform_point(const UT_Vector3 & pos, 
        UT_Vector3 & result)                 = 0;
};

struct DummyInterpolator {
    int data = 1234;
};

template<class Interpol_Type, class Geometry_Type>
class CageDeformer : public DeformerBase<Geometry_Type>
{
public:
    // typedef typename Interpol_Type::InputParametersT ParametersT;
    typedef std::unique_ptr<Interpol_Type> InterpolatorPtr;
     CageDeformer<Interpol_Type, Geometry_Type>() {
        m_interpolator = std::move(InterpolatorPtr(new Interpol_Type()));
     }
    CageDeformer<Interpol_Type, Geometry_Type>(
        const Geometry_Type *mesh, 
        const Geometry_Type *rig, 
        const Geometry_Type *deform_rig) {
        m_interpolator = std::move(InterpolatorPtr(new Interpol_Type())); 
        if (m_interpolator->init(rig, deform_rig)) {
            m_init = true;
        }
    }
    bool init(const Geometry_Type *mesh, const Geometry_Type *rig, 
        const Geometry_Type *deform_rig);
    bool is_init() const { return m_init; }
    bool build(const float & radius);
    bool is_built() const { return m_built; }
    Interpol_Type *  interpolant() const { return m_interpolator.get(); }
    void deform_point(const UT_Vector3 & pos, UT_Vector3 & result);
private:
    bool            m_init  = false;
    bool            m_built = false;
    InterpolatorPtr m_interpolator = nullptr;
    double       m_in_buffer[3]{0,0,0};
    double       m_out_buffer[3]{0,0,0};
};


typedef CageDeformer<DummyInterpolator,       GU_Detail> DummyDeformer;
typedef CageDeformer<ALGLIB_RadialBasisFuncT, GU_Detail> RadialDeformer;

} // end of facedeform namespace

#include "deform.inl"