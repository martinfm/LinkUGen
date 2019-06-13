
#include <ableton/Link.hpp>
#include <chrono>
#include "SC_Unit.h"
#include "SC_PlugIn.h"

static InterfaceTable *ft;

static constexpr double kDefaultQuantum = 4.0;
static double quantum{ kDefaultQuantum };

static ableton::Link* getLinkInstance()
{
  static ableton::Link link(120.0);
  return &link;
}

struct Link : public Unit 
{
};

extern "C" 
{
  void Link_Ctor(Link* unit);
}

void Link_Ctor(Link* unit) 
{
  ableton::Link *link = getLinkInstance();

  float toEnable = IN0(0);

  if (toEnable > 0.0f) {
    float newQuantum = IN0(1);
    quantum = newQuantum;
    
    if (!link->isEnabled()) {
      link->enable(true);
      Print("Ableton Link enabled. Quantum: %f\n", quantum);
    }
    else {
      Print("Ableton Link already enabled. Setting quantum to: %f\n", quantum);
    }
  }
  else { // to disable 
    if (link->isEnabled()) {
      link->enable(false);
      Print("Ableton Link disabled\n");
    }
    else {
      Print("Ableton Link already disabled\n");
    }
  }

  SETCALC(*(ClearUnitOutputs));
  ClearUnitOutputs(unit, 1);
  unit->mDone = true;
}


//-------LinkTempo--------

struct LinkTempo : public Unit 
{
  double mCurTempo;
  double mTempoCalc;
  bool mRunning;
};

extern "C"
{
  void LinkTempo_Ctor(LinkTempo* unit);
  void LinkTempo_next(LinkTempo* unit, int inNumSamples);
}


void LinkTempo_Ctor(LinkTempo* unit) {
  ableton::Link *link = getLinkInstance();

  if (getLinkInstance()->isEnabled()) {
    SETCALC(LinkTempo_next);
  }
  else {
    SETCALC(*(ClearUnitOutputs));
    Print("Ableton Link not enabled! Can't set Link Tempo!\n");
  }

}

void LinkTempo_next(LinkTempo* unit, int inNumSamples)
{
  ableton::Link *link = getLinkInstance();
  const auto sessionState = link->captureAudioSessionState();

  OUT0(0) = float(sessionState.tempo());
}

//-------LinkBeat--------

struct LinkBeat : public Unit 
{
};

extern "C" 
{
  void LinkBeat_Ctor(LinkBeat* unit);
  void LinkBeat_next(LinkBeat* unit, int inNumSamples);
}

void LinkBeat_Ctor(LinkBeat* unit)
{
  ableton::Link *link = getLinkInstance();

  if (link->isEnabled()) {
    SETCALC(LinkBeat_next);
  }
  else {
    SETCALC(*(ClearUnitOutputs));
    Print("Ableton Link not enabled! Cannot create valid LinkBeat\n");
  }
}

void LinkBeat_next(LinkBeat* unit, int inNumSamples)
{

  ableton::Link *link = getLinkInstance();
  const auto sessionState = link->captureAudioSessionState();

  const auto time = link->clock().micros();
  const auto beats = sessionState.beatAtTime(time, quantum);
  OUT0(0) = static_cast<float>(beats);
}

//-------LinkCount-------

struct LinkCount : public Unit 
{
};

extern "C" 
{
  void LinkCount_Ctor(LinkCount* unit);
  void LinkCount_next(LinkCount* unit, int inNumSamples);
}

void LinkCount_Ctor(LinkCount* unit)
{
  ableton::Link *link = getLinkInstance();

  if (link->isEnabled()) {
    SETCALC(LinkCount_next);
  }
  else {
    SETCALC(*(ClearUnitOutputs));
    Print("Ableton Link not enabled! Cannot create valid LinkCount\n");
  }
}

void LinkCount_next(LinkCount* unit, int inNumSamples)
{
  float rate = IN0(0);

  ableton::Link *link = getLinkInstance();
  const auto sessionState = link->captureAudioSessionState();

  const auto time = link->clock().micros();
  const auto phase = sessionState.phaseAtTime(time, quantum);
  
  OUT0(0) = std::floorf(float(phase) * rate);
}

PluginLoad(Link) 
{
  ft = inTable;
  
  DefineSimpleUnit(Link);
  DefineSimpleUnit(LinkTempo);
  DefineSimpleUnit(LinkBeat);
  DefineSimpleUnit(LinkCount);

}

