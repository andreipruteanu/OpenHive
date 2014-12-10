/*
Copyright (c) 2014, Andrei Pruteanu
All rights reserved.

Code is an open source version of HiveKit by Hive-Systems created by 
Andrei Pruteanu, Stefan Dulman and Tomasz Jaskiewicz.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of "Open Hive" nor the names of its
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

#include "types.h"
#include "logging.h"
#include "block_hsl.h"

// constructor
BlockHSL::BlockHSL(runtimeState_t* runtimeState_, uint16_t blockId_) {
	runtimeState = runtimeState_;
	blockId      = blockId_;
}

// do the actual operation
void BlockHSL::out(void) {

    // taken from: https://github.com/ratkins/RGBConverter/blob/master/RGBConverter.cpp
    LOG(LOG_HSL, 2,"HSL2RGB Gate Execute");

    float p, q;

    // retrieve ports and signals pointers from the script datastructure
    float* signals = scriptHandler->getSignals();
    ports_t* ports = scriptHandler->getPorts();

    float h = signals[ports[blockId].in[0]];
    float s = signals[ports[blockId].in[1]];
    float l = signals[ports[blockId].in[2]];

    h = (h < 0 ? 0 : h);
    s = (s < 0 ? 0 : s);
    l = (l < 0 ? 0 : l);
    
    h = (h > 1 ? 1 : h);
    s = (s > 1 ? 1 : s);
    l = (l > 1 ? 1 : l);

    LOG(LOG_HSL,2,"HSL: h=%f, s=%f, l=%f",h, s, l);

    float r,g,b;
    uint8_t rOut, gOut, bOut;

    if (s == 0) {
        r = g = b = l; // achromatic
    } else {
        q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        p = 2 * l - q;
        r = hue2rgb(p, q, h + 1.0/3.0);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.0/3.0);
        
        LOG(LOG_HSL,2,"HSL: q=%f p=%f", q, p);
    }

    LOG(LOG_HSL,2,"HSL: r=%f, g=%f, b=%f",r, g, b);

    rOut = r * 255;
    gOut = g * 255;
    bOut = b * 255;

    // write values into output signals
    signals[ports[blockId].out[0]] = rOut;
    signals[ports[blockId].out[1]] = gOut;
    signals[ports[blockId].out[2]] = bOut;

    LOG(LOG_HSL,2,"HSL: R=%d, G=%d, B=%d",rOut,gOut,bOut);
}

float BlockHSL::hue2rgb(float p, float q, float t) {
    if(t < 0) t += 1;
    if(t > 1) t -= 1;
    if(t < 1.0/6.0) return p + (q - p) * 6.0 * t;
    if(t < 1.0/2.0) return q;
    if(t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6.0;
    return p;
}

// dummy functions needed because of derivation from abstract base class
void BlockHSL::in(void) { }
void BlockHSL::step(void) { }
void BlockHSL::deallocate(void) { }

