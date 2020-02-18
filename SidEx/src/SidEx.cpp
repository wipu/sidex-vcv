#include "plugin.hpp"
#include "sysexconverter.hpp"

struct MidiOutput : dsp::MidiGenerator<PORT_MAX_CHANNELS>, midi::Output {
  void onMessage(midi::Message message) override {
    // not sending normal messages
  }

  void onMessage(uint8_t *bytes, size_t size) override {
    midi::Output::sendMessage(bytes, size);
  }
  
  void reset() {
    Output::reset();
    MidiGenerator::reset();
  }
};

struct SidEx : Module {
  enum ParamIds {
		 NUM_PARAMS
  };
  enum InputIds {
		 GATE1_INPUT,
		 GATE2_INPUT,
		 GATE3_INPUT,
		 FREQ1_INPUT,
		 FREQ2_INPUT,
		 FREQ3_INPUT,
		 WF1_INPUT,
		 WF2_INPUT,
		 WF3_INPUT,
		 PW1_INPUT,
		 PW2_INPUT,
		 PW3_INPUT,
		 RING1_INPUT,
		 RING2_INPUT,
		 RING3_INPUT,
		 SYNC1_INPUT,
		 SYNC2_INPUT,
		 SYNC3_INPUT,
		 A1_INPUT,
		 A2_INPUT,
		 A3_INPUT,
		 D1_INPUT,
		 D2_INPUT,
		 D3_INPUT,
		 S1_INPUT,
		 S2_INPUT,
		 S3_INPUT,
		 R1_INPUT,
		 R2_INPUT,
		 R3_INPUT,
		 FILTER_ENABLE1_INPUT,
		 FILTER_ENABLE2_INPUT,
		 FILTER_ENABLE3_INPUT,
		 FILTER_ENABLE_EXT_INPUT,
		 FILTER_CUTOFF_INPUT,
		 FILTER_RESO_INPUT,
		 FILTER_TYPE_INPUT,
		 MASTER_VOLUME_INPUT,
		 CLOCK_INPUT,
		 NUM_INPUTS
  };
  enum OutputIds {
		  NUM_OUTPUTS
  };
  enum LightIds {
		 NUM_LIGHTS
  };

  MidiOutput midiOutput;
  int timeCounter = 0;
  SidExConverter conv;
  dsp::SchmittTrigger clockTrigger;
  
  SidEx() {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
  }
  
  void process(const ProcessArgs& args) override {
    bool it_is_time = false;
    Port clock = inputs[CLOCK_INPUT];
    if(clock.isConnected()) {
      it_is_time = clockTrigger.process(rescale(clock.getVoltage(), 0.1f, 2.f, 0.f, 1.f));
      timeCounter = 0; // forces count from 0 after disconnecting clock input
    } else {
      timeCounter++;
      int samples_in_frame = args.sampleRate / 50.0;
      if(timeCounter % samples_in_frame == 0) {
	it_is_time = true;
	timeCounter = 0; // no overflow please
      }
    }
    if(it_is_time) {
      
      conv.gate1 = inputs[GATE1_INPUT].getVoltage();
      conv.gate2 = inputs[GATE2_INPUT].getVoltage();
      conv.gate3 = inputs[GATE3_INPUT].getVoltage();

      conv.freq1 = inputs[FREQ1_INPUT].getVoltage();
      conv.freq2 = inputs[FREQ2_INPUT].getVoltage();
      conv.freq3 = inputs[FREQ3_INPUT].getVoltage();

      conv.a1 = inputs[A1_INPUT].getVoltage();
      conv.d1 = inputs[D1_INPUT].getVoltage();
      conv.s1 = inputs[S1_INPUT].getVoltage();
      conv.r1 = inputs[R1_INPUT].getVoltage();
      conv.a2 = inputs[A2_INPUT].getVoltage();
      conv.d2 = inputs[D2_INPUT].getVoltage();
      conv.s2 = inputs[S2_INPUT].getVoltage();
      conv.r2 = inputs[R2_INPUT].getVoltage();
      conv.a3 = inputs[A3_INPUT].getVoltage();
      conv.d3 = inputs[D3_INPUT].getVoltage();
      conv.s3 = inputs[S3_INPUT].getVoltage();
      conv.r3 = inputs[R3_INPUT].getVoltage();

      conv.wf1 = inputs[WF1_INPUT].getVoltage();
      conv.wf2 = inputs[WF2_INPUT].getVoltage();
      conv.wf3 = inputs[WF3_INPUT].getVoltage();

      conv.cutoff = inputs[FILTER_CUTOFF_INPUT].getVoltage();
      conv.reso = inputs[FILTER_RESO_INPUT].getVoltage();
      conv.filt1 = inputs[FILTER_ENABLE1_INPUT].getVoltage();
      conv.filt2 = inputs[FILTER_ENABLE2_INPUT].getVoltage();
      conv.filt3 = inputs[FILTER_ENABLE3_INPUT].getVoltage();
      conv.filtExt = inputs[FILTER_ENABLE_EXT_INPUT].getVoltage();

      conv.filter_mode = inputs[FILTER_TYPE_INPUT].getVoltage();
      
      Port vol = inputs[MASTER_VOLUME_INPUT];
      conv.master_volume = vol.isConnected() ? vol.getVoltage() : 10.0;

      conv.convert();
      
      midiOutput.onMessage(conv.out, SYSEX_DUMP_LENGTH);
    }
  }
};


struct SidExWidget : ModuleWidget {
  SidExWidget(SidEx* module) {
    setModule(module);
    setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SidEx.svg")));
    
    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    
    float oVert = 15.0;
    float oHoriz = 18.15;
    float oy1 = 93.0;
    float oy2 = oy1 + oVert;
    float oy3 = oy2 + oVert;
    float ox = 15.0;
    
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy1)), module, SidEx::GATE1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy2)), module, SidEx::GATE2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy3)), module, SidEx::GATE3_INPUT));
    ox += oHoriz;

    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy1)), module, SidEx::FREQ1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy2)), module, SidEx::FREQ2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy3)), module, SidEx::FREQ3_INPUT));
    ox += oHoriz;
    
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy1)), module, SidEx::WF1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy2)), module, SidEx::WF2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy3)), module, SidEx::WF3_INPUT));
    ox += oHoriz;
    
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy1)), module, SidEx::PW1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy2)), module, SidEx::PW2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy3)), module, SidEx::PW3_INPUT));
    ox += oHoriz;
    
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy1)), module, SidEx::RING1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy2)), module, SidEx::RING2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy3)), module, SidEx::RING3_INPUT));
    ox += oHoriz;
    
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy1)), module, SidEx::SYNC1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy2)), module, SidEx::SYNC2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy3)), module, SidEx::SYNC3_INPUT));
    ox += oHoriz;
    
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy1)), module, SidEx::A1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy2)), module, SidEx::A2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy3)), module, SidEx::A3_INPUT));
    ox += oHoriz;
    
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy1)), module, SidEx::D1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy2)), module, SidEx::D2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy3)), module, SidEx::D3_INPUT));
    ox += oHoriz;
    
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy1)), module, SidEx::S1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy2)), module, SidEx::S2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy3)), module, SidEx::S3_INPUT));
    ox += oHoriz;
    
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy1)), module, SidEx::R1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy2)), module, SidEx::R2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy3)), module, SidEx::R3_INPUT));
    ox += oHoriz;
    
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy1)), module, SidEx::FILTER_ENABLE1_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy2)), module, SidEx::FILTER_ENABLE2_INPUT));
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, oy3)), module, SidEx::FILTER_ENABLE3_INPUT));

    float y = oy1 - oVert * 2;
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, y)), module, SidEx::FILTER_ENABLE_EXT_INPUT));
    y -= oVert;
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, y)), module, SidEx::FILTER_TYPE_INPUT));
    y -= oVert;
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, y)), module, SidEx::FILTER_RESO_INPUT));
    y -= oVert;
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(ox, y)), module, SidEx::FILTER_CUTOFF_INPUT));

    float x = ox - 3*oHoriz;
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y)), module, SidEx::MASTER_VOLUME_INPUT));
    y += oVert;
    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(x, y)), module, SidEx::CLOCK_INPUT));
    
    MidiWidget* midiWidget = createWidget<MidiWidget>(mm2px(Vec(3.41891, 14.8373)));
    midiWidget->box.size = mm2px(Vec(33.840, 28));
    midiWidget->setMidiPort(module ? &module->midiOutput : NULL);
    addChild(midiWidget);
  }
};


Model* modelSidEx = createModel<SidEx, SidExWidget>("SidEx");
