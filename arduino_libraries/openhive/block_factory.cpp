/*
Copyright (c) 2014, Andrei Pruteanu
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the {organization} nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "block.h"
#include "block_factory.h"
#include "block_gradient.h"
#include "block_addition.h"
#include "block_multiplication.h"
#include "block_division.h"
#include "block_substraction.h"
#include "block_negative.h"
#include "block_power.h"
#include "block_absolute.h"
#include "block_intdiv.h"
#include "block_modulus.h"
#include "block_factorial.h"
#include "block_equality.h"
#include "block_similarity.h"
#include "block_smallerthan.h"
#include "block_largerthan.h"
#include "block_gateor.h"
#include "block_gateand.h"
#include "block_gatenot.h"
#include "block_gatexor.h"
#include "block_gatenand.h"
#include "block_gatenor.h"
#include "block_gatexnor.h"
#include "Logging.h"

// factory design pattern
Block* BlockFactory::makeBlock(runtimeState_t* runtimeState_, uint8_t blockName_, uint16_t blockId_) {

	// check which block we want to create
	switch (blockName_) {
		case ALG_SYNC:
			break;
		case ALG_GRADIENT:   // done
			return new BlockGradient(      runtimeState_, blockId_);
			break;
		case ALG_TOKEN:
			break;
    	case ALG_RANDOM:
			break;
    	case ADDITION:       // done
    		return new BlockAddition(      runtimeState_, blockId_);
    		break;
    	case MULTIPLICATION: // done
    		return new BlockMultiplication(runtimeState_, blockId_);
    		break;
    	case DIVISION:       // done
    		return new BlockDivision(      runtimeState_, blockId_);
    		break;
    	case SUBSTRACTION:   // done
    		return new BlockSubstraction(  runtimeState_, blockId_);
    		break;
    	case NEGATIVE:       // done
    		return new BlockNegative(      runtimeState_, blockId_);
			break;
    	case POWER:			 // done
    		return new BlockPower(         runtimeState_, blockId_);
			break;
    	case ABSOLUTE:       // done
    		return new BlockAbsolute(      runtimeState_, blockId_);
			break;
    	case INTDIV:		 // done
    		return new BlockIntDiv(        runtimeState_, blockId_);
			break;
    	case MODULUS:        // done
    		return new BlockModulus(       runtimeState_, blockId_);
			break;
    	case FACTORIAL:      // done
    		return new BlockFactorial(     runtimeState_, blockId_);
			break;
    	case EQUALITY:       // done
    		return new BlockEquality(      runtimeState_, blockId_);
			break;
    	case SIMILARITY:     // done
    		return new BlockSimilarity(    runtimeState_, blockId_);
			break;
    	case SMALLERTHAN:    // done
    		return new BlockSmallerThan(   runtimeState_, blockId_);
			break;
    	case LARGERTHAN:     // done
    		return new BlockLargerThan(    runtimeState_, blockId_);
			break;
    	case GATEOR:         // done
    	    return new BlockGateOr(        runtimeState_, blockId_);
			break;
    	case GATEAND:        // done
    		return new BlockGateAnd(       runtimeState_, blockId_);
			break;
    	case GATENOT:        // done
    		return new BlockGateNot(       runtimeState_, blockId_);
			break;
    	case GATEXOR:        // done
    		return new BlockGateXor(       runtimeState_, blockId_);
			break;
    	case GATENAND:       // done
    		return new BlockGateNand(      runtimeState_, blockId_);
			break;
    	case GATENOR:        // done
    		return new BlockGateNor(       runtimeState_, blockId_);
			break;
    	case GATEMAJORITY:
			break;
    	case GATEXNOR:       // done
    		return new BlockGateXnor(      runtimeState_, blockId_);
			break;
    	case SIN:
			break;
    	case SENSOR:
			break;
    	case ACTUATOR:
			break;
    	case HOLD:
			break;
    	case TICK:
			break;
    	case DELAY:
			break;
    	case HSL2RGB:
			break;
    	case COMBINE:
			break;
    	case SCALE:
			break;
    	case LOGIC:
			break;
    	case FLASHWRITE:
			break;
    	case FLASHREAD:
			break;
		default:
			LOG(LOG_PRINT,1,"Got unknown block type. Check the binary !\n");
			break;
	}
}

// overloading "=" operator
//Block& Block::operator=(Block const &rhs) {
//	return *this;
//}