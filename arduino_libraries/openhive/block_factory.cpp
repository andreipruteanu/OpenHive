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
#include "block_sin.h"
#include "block_hsl.h"
#include "block_combine.h"
#include "block_scale.h"
#include "block_majority.h"
#include "block_delay.h"
#include "block_token.h"
#include "Logging.h"

// factory design pattern
Block* BlockFactory::makeBlock(runtimeState_t* runtimeState_, uint8_t blockName_, uint16_t blockId_) {

	// check which block we want to create
	switch (blockName_) {
		case ALG_SYNC:			break;  // TODO
		case ALG_GRADIENT:   return new BlockGradient(      runtimeState_, blockId_);   break; // done
		case ALG_TOKEN:		 return new BlockToken(         runtimeState_, blockId_);   break; // done
    	case ALG_RANDOM:		break;  // TODO
    	case ADDITION:       return new BlockAddition(      runtimeState_, blockId_);   break; // done
    	case MULTIPLICATION: return new BlockMultiplication(runtimeState_, blockId_);   break; // done
    	case DIVISION:       return new BlockDivision(      runtimeState_, blockId_);   break; // done
    	case SUBSTRACTION:   return new BlockSubstraction(  runtimeState_, blockId_);   break; // done
    	case NEGATIVE:       return new BlockNegative(      runtimeState_, blockId_);   break; // done
    	case POWER:          return new BlockPower(         runtimeState_, blockId_);   break; // done
    	case ABSOLUTE:       return new BlockAbsolute(      runtimeState_, blockId_);   break; // done
    	case INTDIV:         return new BlockIntDiv(        runtimeState_, blockId_);   break; // done
    	case MODULUS:        return new BlockModulus(       runtimeState_, blockId_);   break; // done
    	case FACTORIAL:      return new BlockFactorial(     runtimeState_, blockId_);   break; // done
    	case EQUALITY:       return new BlockEquality(      runtimeState_, blockId_);   break; // done
    	case SIMILARITY:     return new BlockSimilarity(    runtimeState_, blockId_);   break; // done
    	case SMALLERTHAN:    return new BlockSmallerThan(   runtimeState_, blockId_);   break; // done
    	case LARGERTHAN:     return new BlockLargerThan(    runtimeState_, blockId_);   break; // done
    	case GATEOR:         return new BlockGateOr(        runtimeState_, blockId_);   break; // done
    	case GATEAND:        return new BlockGateAnd(       runtimeState_, blockId_);   break; // done
    	case GATENOT:        return new BlockGateNot(       runtimeState_, blockId_);   break; // done
    	case GATEXOR:        return new BlockGateXor(       runtimeState_, blockId_);   break; // done
    	case GATENAND:       return new BlockGateNand(      runtimeState_, blockId_);   break; // done
    	case GATENOR:        return new BlockGateNor(       runtimeState_, blockId_);   break; // done
    	case GATEMAJORITY:   return new BlockMajority(      runtimeState_, blockId_);   break; // done
    	case GATEXNOR:       return new BlockGateXnor(      runtimeState_, blockId_);   break; // done
    	case SIN:            return new BlockSin(           runtimeState_, blockId_);   break; // done
    	case SENSOR:            break;  // TODO
    	case ACTUATOR:			break;  // TODO
    	case HOLD:				break;  // TODO
    	case TICK:				break;  // TODO
    	case DELAY:			 return new BlockDelay(         runtimeState_, blockId_);   break; // done
    	case HSL2RGB:        return new BlockHSL(           runtimeState_, blockId_);   break; // done
    	case COMBINE:        return new BlockCombine(       runtimeState_, blockId_);   break; // done
    	case SCALE:          return new BlockScale(         runtimeState_, blockId_);   break; // done
    	case LOGIC:             break;  // TODO
    	case FLASHWRITE:        break;  // TODO
    	case FLASHREAD:			break;  // TODO
		default:			LOG(LOG_PRINT,1,"Got unknown block type !\n");              break; // done
	}
}

// overloading "=" operator
//Block& Block::operator=(Block const &rhs) {
//	return *this;
//}