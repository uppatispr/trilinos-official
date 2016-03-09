// @HEADER
// ************************************************************************
//
//                           Intrepid2 Package
//                 Copyright (2007) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Kyungjoo Kim  (kyukim@sandia.gov), or
//                    Mauro Perego  (mperego@sandia.gov)
//
// ************************************************************************
// @HEADER

#include "mex.h"
#include "Intrepid2_FunctionSpaceTools.hpp"
#include "m2i_helpers.hpp"

using namespace Intrepid2;

void mexFunction(int nOutput, mxArray *pOutput[], int nInput, const mxArray *pInput[])
{

    std::string descriptor =
            ("\nintrepid_computeCellMeasure ..... MEX interface for the Intrepid (Trilinos) function Intrepid2::FunctionSpaceTools::computeCellMeasure.\n\n"
                    "\tintrepid_computeCellMeasure(outMeasures,inDets,inWeights)\n\n"
                    "\t<1-in/out> outMeasures = Weighted measures (2D array of size [#cubPoints x #cells])\n"
                    "\t<2-in>     inDets = Determinants of cell Jacobians at cubature points (2D array of size [#cubPoints x #cells])\n"
                    "\t<3-in>     inWeights = Cubature weights (1D array of size [#cubPoints])\n\n");

    // Check the number of input arguments
    if(nInput != 3)
    {
        std::string ioError = descriptor + "Incorrect number of input arguments!!!\n";
        mexErrMsgTxt(ioError.c_str());
    }
    if(nOutput != 0)
    {
        std::string ioError = descriptor + "There can be no output arguments!!!\n";
        mexErrMsgTxt(ioError.c_str());
    }

    // Get the dimensions of the measures array
    Teuchos::Array<int> measures_dims;
    m2iGetArrayDims(measures_dims, pInput[0]);
    // Get the dimensions of the determinant array
    Teuchos::Array<int> dets_dims;
    m2iGetArrayDims(dets_dims, pInput[1]);
    // Get the dimensions of the weights array
    Teuchos::Array<int> weights_dims;
    m2iGetArrayDims(weights_dims, pInput[2], true);

    // Get the (pointers to) data
    double* measures_raw = mxGetPr(pInput[0]);
    double* dets_raw = mxGetPr(pInput[1]);
    double* weights_raw = mxGetPr(pInput[2]);

    FieldContainer<double> cellMeasures(measures_dims, measures_raw);
    FieldContainer<double> cellDets(dets_dims, dets_raw);
    FieldContainer<double> cubWeights(weights_dims, weights_raw);

    try
    {
        FunctionSpaceTools::computeCellMeasure<double>(cellMeasures, cellDets, cubWeights);
    } catch(const std::exception &e)
    {
        std::string intrepiderr = e.what();
        std::string matlaberr = "------------------------------------------------------------\n"
                + ("MATLAB returned:  Invalid arguments in the call to intrepid_computeCellMeasure.\n"
                        + ("Intrepid (Trilinos) returned:\n" + intrepiderr));
        mexErrMsgTxt(matlaberr.c_str());
    }
}
