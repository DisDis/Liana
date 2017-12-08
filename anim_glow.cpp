#include "anim_glow.h"

void AnimGlow::processGlow(int i)
{
    int bra = (char) (braPhase + i * braFreq);
    bra = BRA_OFFSET + (abs(bra) >> BRA_AMP_SHIFT);
    leds[i] = leds[i].brightness((int)bra);
}

void AnimGlow::runImpl()
{
    braPhase += braPhaseSpd;   
}