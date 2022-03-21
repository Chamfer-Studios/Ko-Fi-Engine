#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include "imgui.h"

#include <string>
#include <vector>

#include "al.h"
#include "alc.h"
#include "alext.h"
#include "efx.h"
#include "efx-creative.h"
#include "efx-presets.h"

class Effect
{
public:
	virtual ~Effect() {}

	virtual void Start()
	{
	}

	virtual void Update()
	{
	}

	virtual void Draw()
	{
	}

	virtual void End()
	{

	}

public: // Methods

	const char* GetName() const
	{
		return name.c_str();
	}

	void ToggleBypass(bool toggle)
	{
		toggle ? alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, (ALint)effect) : alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
		nobypass = toggle;

	}

	void AddHelper(const char* title = "(?)", const char* desc = "", bool sameLine = false )
	{
		if (sameLine) ImGui::SameLine();
		ImGui::Text(title);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}


	void Disconnect(ALuint source)
	{
		alSource3i(source, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
		//alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
		alDeleteAuxiliaryEffectSlots(1, &slot);
		alDeleteEffects(1, &effect);
	}

public: // Variables

	bool selected = false;
	bool nobypass = true;

protected:

	Effect(const char* xname, ALint ID, bool bypass = false)
	{
		name.clear();
		name = xname;
		id = ID;
		nobypass = !bypass;
	}

	void Generate(ALuint source = 0, bool bypass = false)
	{
		/*ALCdevice* device = alcOpenDevice(NULL);
		const ALCchar* string = alcGetString(device, ALC_DEVICE_SPECIFIER);*/

#define LOAD_PROC(T, x)  ((x) = (T)alGetProcAddress(#x))
		LOAD_PROC(LPALGENEFFECTS, alGenEffects);
		LOAD_PROC(LPALDELETEEFFECTS, alDeleteEffects);
		LOAD_PROC(LPALISEFFECT, alIsEffect);
		LOAD_PROC(LPALEFFECTI, alEffecti);
		LOAD_PROC(LPALEFFECTIV, alEffectiv);
		LOAD_PROC(LPALEFFECTF, alEffectf);
		LOAD_PROC(LPALEFFECTFV, alEffectfv);
		LOAD_PROC(LPALGETEFFECTI, alGetEffecti);
		LOAD_PROC(LPALGETEFFECTIV, alGetEffectiv);
		LOAD_PROC(LPALGETEFFECTF, alGetEffectf);
		LOAD_PROC(LPALGETEFFECTFV, alGetEffectfv);

		LOAD_PROC(LPALGENAUXILIARYEFFECTSLOTS, alGenAuxiliaryEffectSlots);
		LOAD_PROC(LPALDELETEAUXILIARYEFFECTSLOTS, alDeleteAuxiliaryEffectSlots);
		LOAD_PROC(LPALISAUXILIARYEFFECTSLOT, alIsAuxiliaryEffectSlot);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTI, alAuxiliaryEffectSloti);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTIV, alAuxiliaryEffectSlotiv);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTF, alAuxiliaryEffectSlotf);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTFV, alAuxiliaryEffectSlotfv);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTI, alGetAuxiliaryEffectSloti);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTIV, alGetAuxiliaryEffectSlotiv);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTF, alGetAuxiliaryEffectSlotf);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTFV, alGetAuxiliaryEffectSlotfv);
#undef LOAD_PROC

		alGenEffects(1, &effect);
		alEffecti(effect, AL_EFFECT_TYPE, id); // Error on the id
		if (alGetError() == AL_INVALID_VALUE)
		{
			printf("Reverb Effect not supported\n");
		}
		alGenAuxiliaryEffectSlots(1, &slot);
		ToggleBypass(!bypass);
		alSource3i(source, AL_AUXILIARY_SEND_FILTER, (ALint)slot, 0, AL_FILTER_NULL);
	}

	void SetEffectValue(ALuint param, float value)
	{
		alEffectf(effect, param, value);
		alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, (ALint)effect);
	}

	std::string name = "Effect";
	ALuint effect = 0, slot = 0;
	ALint id = 0;

	/* Effect object functions */
	LPALGENEFFECTS alGenEffects;
	LPALDELETEEFFECTS alDeleteEffects;
	LPALISEFFECT alIsEffect;
	LPALEFFECTI alEffecti;
	LPALEFFECTIV alEffectiv;
	LPALEFFECTF alEffectf;
	LPALEFFECTFV alEffectfv;
	LPALGETEFFECTI alGetEffecti;
	LPALGETEFFECTIV alGetEffectiv;
	LPALGETEFFECTF alGetEffectf;
	LPALGETEFFECTFV alGetEffectfv;

	/* Auxiliary Effect Slot object functions */
	LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
	LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
	LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
	LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
	LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
	LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
	LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
	LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
	LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
	LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
	LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;
};

class Reverb : public Effect
{
public:
	Reverb(ALuint source, bool bypass) : Effect("Reverb", AL_EFFECT_EAXREVERB, bypass)
	{
		Generate(source, bypass);
	}
	~Reverb()
	{
	}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Reverb"); ImGui::SameLine();

		if (ImGui::Checkbox("##BypassR", &nobypass)) ToggleBypass(nobypass);

		ImGui::PushItemWidth(70.0f);
		ImGui::BeginDisabled(!nobypass);

		//REVERB
		ImGui::SliderFloat("##ReverbWet", &gain, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_GAIN, gain);
		AddHelper("Wet", "WET PROPERTY:\nThe Wet property controls the volume of the reverb track. Setting it to maximum will set full reverb track.\nSetting it to minimum will not apply reverb", true);
		ImGui::SameLine(); ImGui::Text("      "); ImGui::SameLine();

		//DIFFUSION
		ImGui::SliderFloat("##ReverbDiffusion  ", &diffusion, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_DIFFUSION, diffusion);
		AddHelper("Diffusion  ", "DIFFUSION PROPERTY:\nThe Reverb Diffusion property controls the echo density in the reverberation decay.\nReducing diffusion gives the reverberation a more “grainy” character that is especially noticeable with percussive sound sources.\nIf you set a diffusion value of 0.0, the later reverberation sounds like a succession of distinct echoes.", true);
		ImGui::SameLine();

		//ECHO TIME
		ImGui::SliderFloat("##ReverbEchoTime", &echoTime, 0.075f, 0.25f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_ECHO_TIME, echoTime);
		AddHelper("Echo Time", "ECHO TIME PROPERTY:\nEcho Time controls the rate at which the cyclic echo repeats itself along the reverberation decay.\nFor example, the default setting for Echo Time is 250 ms, causing the echo to occur 4 times per second.\nTherefore, if you were to clap your hands in this type of environment, you will hear four repetitions of clap per second.", true);
		ImGui::SameLine(); ImGui::Text(""); ImGui::SameLine();

		//REFLECT GAIN
		ImGui::SliderFloat("##ReverbRflcGain", &reflectionGain, 0.0f, 3.16f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_REFLECTIONS_GAIN, reflectionGain);
		AddHelper("Rflc Gain", "REFLECT GAIN PROPERTY:\nThe Reflections Gain property controls the overall amount of initial reflections relative to the Gain property.\nYou can increase the amount of initial reflections to simulate a more narrow space or closer walls,\nespecially effective if you associate the initial reflections increase with a reduction in reflections delays by lowering the value of the Reflection Delay property.\nTo simulate open or semi - open environments, you can maintain the amount of early reflections while reducing the value of the Late Reverb Gain property, which controls later reflections. ", true);
		ImGui::Spacing();

		//DECAY
		ImGui::SliderFloat("##ReverbDecay", &decayTime, 0.1f, 20.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_DECAY_TIME, decayTime);
		AddHelper("Decay", "DECAY PROPERTY:\nThe Decay Time property sets the reverberation decay time.\nMinimum value sets typical small room with very dead surface.\n Maximum value sets typical large room with very live surfaces.", true);
		ImGui::SameLine(); ImGui::Text("    "); ImGui::SameLine();

		//DENSITY
		ImGui::SliderFloat("##ReverbDensity", &density, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_DENSITY, density);
		AddHelper("Density", "DENSITY PROPERTY:\nReverb Density property controls the coloration of the late reverb.\nLowering the value adds more coloration to the late reverb.", true);
		ImGui::SameLine(); ImGui::Text("   "); ImGui::SameLine();

		//ECHO DEPTH
		ImGui::SliderFloat("##ReverbEchoDepth", &echoDepth, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_ECHO_DEPTH, echoDepth);
		AddHelper("Echo Depth", "ECHO DEPTH PROPERTY:\nEcho Depth property introduces a cyclic echo in the reverberation decay, which will be noticeable with transient or percussive sounds.\nA larger value of Echo Depth will make this effect more prominent.", true);
		ImGui::SameLine();

		//REFLECT DELAY
		ImGui::SliderFloat("##ReverbReflectDelay", &reflectionDelay, 0.0f, 0.3f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_REFLECTIONS_DELAY, reflectionDelay);
		AddHelper("Rflc Delay", "REFLECT DELAY PROPERTY:\nThe Reflections Delay property is the amount of delay between the arrival time of the direct path from the source to the first reflection from the source.\nYou can reduce or increase Reflections Delay to simulate closer or more distant reflective surfaces\nand therefore control the perceived size of the room.", true);
		ImGui::Spacing();

		//DELAY TIME
		ImGui::SliderFloat("##ReverbDelayTime", &lateReverbDelay, 0.0f, 0.1f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_LATE_REVERB_DELAY, lateReverbDelay);
		AddHelper("Delay Time", "DELAY TIME PROPERTY:\nThe Late Reverb Delay property defines the begin time of the late reverberation relative to the\ntime of the initial reflection (the first of the early reflections).\nReducing or increasing Late Reverb Delay is useful for simulating a smaller or larger room.", true);
		ImGui::SameLine();

		//DELAY GAIN
		ImGui::SliderFloat("##ReverbDelayGain", &lateReverbGain, 0.0f, 10.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_LATE_REVERB_GAIN, lateReverbGain);
		AddHelper("Delay Gain", "DELAY GAIN PROPERTY:\nThe Delay Gain property controls the overall amount of later reverberation relative to the Gain property.\nNote that Delay Gain and Decay Time are independent properties:\nIf you adjust Decay Time without changing Delay Gain, the total intensity (the averaged square of the amplitude) of the late reverberation remains constant.", true);
		ImGui::SameLine(); ImGui::Text(""); ImGui::SameLine();

		//MOD TIME
		ImGui::SliderFloat("##ReverbModTime", &modulationTime, 0.004f, 4.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_MODULATION_TIME, modulationTime);
		AddHelper("Mod Time", "MODULATION TIME PROPERTY:\nModulation Time controls the speed of the vibrato (rate of periodic changes in pitch).", true);
		ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine();

		//MOD DEPTH
		ImGui::SliderFloat("##ReverbModDepth", &modulationDepth, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_MODULATION_DEPTH, modulationDepth);
		AddHelper("Mod Time", "MODULATION DEPTH PROPERTY:\nModulation Depth controls the amount of pitch change.\nLow values of Diffusion will contribute to reinforcing the perceived effect by reducing the mixing of overlapping reflections in the reverberation decay.", true);
		ImGui::SameLine(); ImGui::Text(""); ImGui::SameLine();

		ImGui::EndDisabled();
		ImGui::PopItemWidth();
	}

	void End()
	{

	}

private: // Methods

private: // Variables

	float density = 1.0f, diffusion = 1.0f, gain = 0.32f, /*gainhf, gainlf,*/ decayTime = 1.49f,
		/*decayhfRatio, decaylfRatio,*/ reflectionGain = 0.05f, reflectionDelay = 0.007f,
		lateReverbGain = 1.26f, lateReverbDelay = 0.011f, echoTime = 0.25f, echoDepth = 0.0f,
		modulationTime = 0.25f, modulationDepth = 0.0f/*, airAbsortion, roomRolloff,
		decayhfLimit, hfreference, lfreference*/;

};

class Delay : public Effect
{
public:
	Delay(ALint source, bool bypass) : Effect("Delay", AL_EFFECT_ECHO)
	{
		Generate(source, bypass);
	}
	~Delay() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Delay"); ImGui::SameLine();
		if (ImGui::Checkbox("##BypassDl", &nobypass)) ToggleBypass(nobypass);

		ImGui::BeginDisabled(!nobypass);
		ImGui::PushItemWidth(185.0f);

		ImGui::SliderFloat("##DelayFeedback", &feedback, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_ECHO_FEEDBACK, feedback);
		AddHelper("Feedback", "FEEDBACK PROPERTY:\nThis property controls how hard panned the individual echoes are.\nWith maxim value, the first ‘tap’ will be panned hard left, and the second tap hard right.\nA minim value gives the opposite result.Settings nearer to 0.0 result in less emphasized panning.", true);
		ImGui::SameLine(); ImGui::Text("      "); ImGui::SameLine(0.f, 4.0f);

		ImGui::SliderFloat("##DelaySpread", &spread, -1.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_ECHO_SPREAD, spread);
		AddHelper("Spread", "SPREAD PROPERTY:\nThis property controls the amount of feedback the output signal fed back into the input.\nUse this parameter to create “cascading” echoes. At full magnitude, the identical sample will repeat endlessly.\nBelow full magnitude, the sample will repeat and fade.", true);
		ImGui::Spacing();

		ImGui::SliderFloat("##DelayTime", &delaylr, 0.0f, 0.404f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_ECHO_LRDELAY, delaylr);
		AddHelper("Time", "TIME PROPERTY:\nThis property controls the delay between the first ‘tap’ and the second ‘tap’.\nSubsequently, the value for Delay Time is used to determine the time delay between each ‘first tap’ and the next ‘second tap’.", true);
		ImGui::SameLine(); ImGui::Text("      "); ImGui::SameLine(0.f, 4.0f);

		ImGui::SliderFloat("##DelayOffset", &delay, 0.0f, 0.207f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_ECHO_DELAY, delay);
		AddHelper("Offset", "OFFSET PROPERTY:\nThis property controls the offset delay between the original sound and the first ‘tap’, or echo instance.\nSubsequently, the value for DELAY OFFSET is used to determine the time delay between each ‘second tap’and the next ‘first tap’.", true);
		ImGui::Spacing();

		ImGui::SliderFloat("##DelayDamping", &damping, 0.0f, 0.99f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_ECHO_DAMPING, damping);
		AddHelper("Damping", "DAMPING PROPERTY:\nThis property controls the amount of high frequency damping applied to each echo.\nAs the sound is subsequently fed back for further echoes, damping results in an echo which progressively gets softer in tone as well as intensity.", true);

		ImGui::EndDisabled();
		ImGui::PopItemWidth();
	}

	void End()
	{

	}

private: // Methods

private: // Variables

	float feedback = 0.5f, spread = -1.0f, delaylr = 0.1f,
		delay = 0.1f, damping = 0.5f;

};

class Distortion : public Effect
{
public:
	Distortion(ALint source, bool bypass) : Effect("Distortion", AL_EFFECT_DISTORTION)
	{
		Generate(source, bypass);
	}
	~Distortion() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Distortion"); ImGui::SameLine();
		if (ImGui::Checkbox("##BypassD", &nobypass)) ToggleBypass(nobypass);

		ImGui::BeginDisabled(!nobypass);
		ImGui::PushItemWidth(130.0f);

		//WET
		ImGui::SliderFloat("##DistortionWet", &gain, 0.01f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_GAIN, gain);
		AddHelper("Wet", "WET PROPERTY:\nThe Wet property controls the volume of the distorted track. Setting it to maximum will set full distortion track.\nSetting it to minimum will not apply distortion", true);
		ImGui::SameLine(); ImGui::Text("      "); ImGui::SameLine(0.f, 4.0f);

		ImGui::SliderFloat("##DistortionEdge", &edge, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_EDGE, edge);
		AddHelper("Edge", "EDGE PROPERTY:\nThis property controls the shape of the distortion.\nThe higher the value for Edge, the ‘dirtier’ and ‘fuzzier’ the effect.", true);
		ImGui::SameLine(); ImGui::Text("     "); ImGui::SameLine(0.f, 4.0f);

		ImGui::SliderFloat("##DistortionLPCutoff", &lowpassCutoff, 80.0f, 24000.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_LOWPASS_CUTOFF, lowpassCutoff);
		AddHelper("LP Cutoff", "LOW PASS CUTOFF PROPERTY:\nInput signal can have a low pass filter applied,\nto limit the amount of high frequency signal feeding into the distortion effect.", true);
		ImGui::Spacing();

		ImGui::SliderFloat("##DistortionEQCenter", &eqCenter, 80.0f, 24000.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_EQCENTER, eqCenter);
		AddHelper("EQ Center", "EQ CENTER PROPERTY:\nThis property controls the frequency at which the post-distortion attenuation (Distortion Gain) is active.", true);
		ImGui::SameLine(); ImGui::Text(""); ImGui::SameLine(0.f, 4.0f);

		ImGui::SliderFloat("##DistortionEQBandW ", &eqBandWidth, 80.0f, 24000.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_EQBANDWIDTH, eqBandWidth);
		AddHelper("EQ Band W", "EQ BAND WIDTH PROPERTY:\nThis property controls the bandwidth of the post-distortion attenuation.", true);

		ImGui::EndDisabled();
		ImGui::PopItemWidth();
	}

	void End()
	{

	}

private: // Methods

private: // Variables

	float edge = 0.2f, eqBandWidth = 3600.0f, gain = 0.05f,
		eqCenter = 3600.0f, lowpassCutoff = 8000.0f;

};

class Flanger : public Effect
{
public:
	Flanger(ALint source, bool bypass) : Effect("Flanger", AL_EFFECT_FLANGER)
	{
		Generate(source, bypass);
	}
	~Flanger() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Flanger"); ImGui::SameLine();
		if (ImGui::Checkbox("##BypassF", &nobypass)) ToggleBypass(nobypass);

		ImGui::BeginDisabled(!nobypass);
		ImGui::PushItemWidth(130.0f);

		ImGui::SliderFloat("##FlangerDepth", &depth, 0.01f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_FLANGER_DEPTH, depth);
		AddHelper("Depth", "DEPTH PROPERTY:\nThe ratio by which the delay time is modulated by the LFO.\nUse this parameter to increase the pitch modulation.", true);
		ImGui::SameLine(); ImGui::Text("    "); ImGui::SameLine(0.f, 4.0f);

		ImGui::SliderFloat("##FlangerFeedback", &feedback, -1.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_FLANGER_FEEDBACK, feedback);
		AddHelper("Feedback", "FEEDBACK PROPERTY:\nThis is the amount of the output signal level fed back into the effect’s input.\nA negative value will reverse the phase of the feedback signal.\nUse this parameter to create an “intense metallic” effect.\nAt full magnitude, the identical sample will repeat endlessly.\nAt less than full magnitude, the sample will repeat and fade out over time. ", true);
		ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine(0.f, 4.0f);

		ImGui::SliderFloat("##FlangerDelay     ", &delay, 0.0f, 0.004f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_FLANGER_DELAY, delay);
		AddHelper("Delay", "DELAY PROPERTY:\nThe average amount of time the sample is delayed before it is played back; with feedback, the amount of time between iterations of the sample. ", true);
		ImGui::Spacing();

		ImGui::SliderFloat("##FlangerRate", &rate, 0.0f, 10.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_FLANGER_RATE, rate);
		AddHelper("Rate", "RATE PROPERTY:\nThe number of times per second the LFO controlling the amount of delay repeats.\nHigher values increase the pitch modulation.", true);
		ImGui::SameLine(); ImGui::Text("     "); ImGui::SameLine(0.f, 4.0f);

		if (ImGui::Combo("##FlangerWaveform", &waveformIndex, &waveform[0], waveform.size()))
		{
			ALint wave = AL_FLANGER_WAVEFORM_TRIANGLE;
			waveformIndex == 0 ? wave = AL_FLANGER_WAVEFORM_SINUSOID : wave = AL_FLANGER_WAVEFORM_TRIANGLE;
			SetEffectValue(AL_FLANGER_WAVEFORM, wave);
		}
		AddHelper("Waveform", "WAVEFORM PROPERTY:\nSelects the shape of the LFO waveform that controls the amount of the delay of the sampled signal.Zero is a sinusoid and one is a triangle.", true);
		ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine(0.f, 4.0f);

		ImGui::SliderFloat("##FlangerPhase", &phase, -180.0f, 180.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_FLANGER_PHASE, phase);
		AddHelper("Phase", "PHASE PROPERTY:\nThis changes the phase difference between the left and right LFO’s.\nAt zero degrees the two LFOs are synchronized.", true);

		ImGui::EndDisabled();
		ImGui::PopItemWidth();
	}

	void End()
	{

	}

private: // Methods

private: // Variables

	std::vector<const char*> waveform = { "Sinoidal", "Triangular" };
	int waveformIndex = 0;

	float depth = 1.0f, feedback = -0.5f, delay = 0.002f, 
		rate = 0.27f, phase = 0.0f;
};

class Chorus : public Effect
{
public:
	Chorus(ALint source, bool bypass) : Effect("Chorus", AL_EFFECT_CHORUS)
	{
		Generate(source, bypass);
	}
	~Chorus() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Chorus"); ImGui::SameLine();
		if (ImGui::Checkbox("##BypassC", &nobypass)) ToggleBypass(nobypass);

		ImGui::BeginDisabled(!nobypass);
		ImGui::PushItemWidth(130.0f);

		ImGui::SliderFloat("##ChorusDepth", &depth, 0.01f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_CHORUS_DEPTH, depth);
		AddHelper("Depth", "DEPTH PROPERTY:\nThis property controls the amount of processed signal that is fed back to the input of the chorus effect.\nNegative values will reverse the phase of the feedback signal. At full magnitude theidentical sample will repeat endlessly.\nAt lower magnitudes the sample will repeat and fade out over time.\nUse this parameter to create a “cascading” chorus effect.", true);
		ImGui::SameLine(); ImGui::Text("    "); ImGui::SameLine(0.f, 4.0f);

		ImGui::SliderFloat("##ChorusFeedback", &feedback, -1.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_CHORUS_FEEDBACK, feedback);
		AddHelper("Feedback", "FEEDBACK PROPERTY:\nThis is the amount of the output signal level fed back into the effect’s input.\nA negative value will reverse the phase of the feedback signal.\nUse this parameter to create an “intense metallic” effect.\nAt full magnitude, the identical sample will repeat endlessly.\nAt less than full magnitude, the sample will repeat and fade out over time. ", true);
		ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine(0.f, 4.0f);

		ImGui::SliderFloat("##ChorusDelay     ", &delay, 0.0f, 0.004f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_CHORUS_DELAY, delay);
		AddHelper("Delay", "DELAY PROPERTY:\nThis property controls the average amount of time the sample is delayed before it is played back, and with feedback, the amount of time between iterations of the sample. Larger values lower thepitch.\nSmaller values make the chorus sound like a flanger, but with different frequency characteristics. ", true);
		ImGui::Spacing();

		ImGui::SliderFloat("##ChorusRate", &rate, 0.0f, 10.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_CHORUS_RATE, rate);
		AddHelper("Rate", "RATE PROPERTY:\nThis property sets the modulation rate of the LFO that controls the delay time of the delayed signals. ", true);
		ImGui::SameLine(); ImGui::Text("     "); ImGui::SameLine(0.f, 4.0f);

		if (ImGui::Combo("##ChorusWaveform", &waveformIndex, &waveform[0], waveform.size()))
		{
			ALint wave = AL_CHORUS_WAVEFORM_TRIANGLE;
			waveformIndex == 0 ? wave = AL_CHORUS_WAVEFORM_SINUSOID : wave = AL_CHORUS_WAVEFORM_TRIANGLE;
			SetEffectValue(AL_CHORUS_WAVEFORM, wave);
		}
		AddHelper("Waveform", "WAVEFORM PROPERTY:\nThis property sets the waveform shape of the LFO that controls the delay time of the delayed signals.", true);
		ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine(0.f, 4.0f);

		ImGui::SliderFloat("##ChorusPhase", &phase, -180.0f, 180.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_CHORUS_PHASE, phase);
		AddHelper("Phase", "PHASE PROPERTY:\nThis property controls the phase difference between the left and right LFO’s.\nAt zero degrees the two LFOs are synchronized.\nUse this parameter to create the illusion of an expanded stereo field of the output signal. ", true);

		ImGui::EndDisabled();
		ImGui::PopItemWidth();
	}

	void End()
	{

	}

private: // Methods

private: // Variables

	std::vector<const char*> waveform = { "Sinoidal", "Triangular" };
	int waveformIndex = 0;

	float depth = 0.1f, feedback = 0.25f, delay = 0.016f,
		rate = 1.1f, phase = 90.0f;
};

#endif // !__EFFECTS_H__