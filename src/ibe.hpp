#ifndef IBE_H
#define IBE_H

extern "C"{
    #include <dclxvi/curvepoint_fp.h>
    #include <dclxvi/twistpoint_fp2.h>
    #include <dclxvi/fp2e.h>
    #include <dclxvi/fp12e.h>
}

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>

#include <boost/random.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/multiprecision/integer.hpp>

#include <gmp.h>
#include <string>
#include <functional>
#include <array>
#include <vector>
#include <algorithm>
#include <iterator>
#include <time.h>
#include "conv.hpp"
#include "utils.hpp"

//namespace mp = boost::multiprecision; 
//namespace rd = boost::random;

extern curvepoint_fp_t bn_curvegen;

extern Int order;

using Int = boost::multiprecision::cpp_int;

typedef curvepoint_fp_t dclxvi_cp;
typedef twistpoint_fp2_t twis;


struct g2 {
	twistpoint_fp2_t p;
};

struct g1 {
	curvepoint_fp_t p;
};

struct MasterPublicKey{
    dclxvi_cp *g1;
};

struct MasterPrivateKey{
    Int s;
};

struct IdentityPrivateKey{
    twistpoint_fp2_t *d;
    twistpoint_fp2_t *q;
};

typedef struct MasterPrivateKey mpriv;
typedef struct MasterPublicKey mpublic;
typedef struct IdentityPrivateKey idpk;

class MasterKey
{
public:
    MasterKey();
    void setup();
    void extract(std::string id);
    mpriv private_key;
    mpublic public_key; 
    idpk id_private_key;
    fp2e_t twistB;
    Int twist_cofactor;
private:
    Int twist_order;
};

void Set_xy_twistpoint(twistpoint_fp2_t & rop, fp2e_t x, fp2e_t y);

template<class Integer>
void Set_xy_fp2e(Integer x, Integer y, fp2e_t& b);

void hash_to_point(std::string m, twistpoint_fp2_t& pt);
void hash_to_twist_subgroup(std::string m, twistpoint_fp2_t& pt);
void hashtotwistpoint(std::string m, twistpoint_fp2_t& pt);

#endif