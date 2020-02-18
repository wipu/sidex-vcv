const size_t SYSEX_DUMP_LENGTH = 35;

// from mockodore-model-sid SidRegisterAddresses.allInDefaultWritingOrder
// TODO generate from it
enum SidRegister
  {
   AD1,
   SR1,
   FreqHi1,
   FreqLo1,
   PwHi1,
   PwLo1,
   CR1,
   AD2,
   SR2,
   FreqHi2,
   FreqLo2,
   PwHi2,
   PwLo2,
   CR2,
   AD3,
   SR3,
   FreqHi3,
   FreqLo3,
   PwHi3,
   PwLo3,
   CR3,
   FcHi,
   FcLo,
   ModeVol,
   ResFilt,
   Padding0, // length must be multiple of 7 for sysex encoding
   Padding1,
   Padding2,
   NUM_SID_REGISTERS,
  };

int freqInt(float freq) {
  // TODO calculate proper V/oct values for SID
  // so that default offset + 0cv = C4

  float f = freq;
  if(f < 0 ) f = 0;
  float as16bit = (f / 10.0) * 65535;
  
  int retval = (int) as16bit;
  return retval;
}

struct SidExConverter {

  float gate1 = 0.0;
  float gate2 = 0.0;
  float gate3 = 0.0;
  
  float freq1 = 0.0;
  float freq2 = 0.0;
  float freq3 = 0.0;

  float a1 = 0.0;
  float d1 = 0.0;
  float s1 = 0.0;
  float r1 = 0.0;
  float a2 = 0.0;
  float d2 = 0.0;
  float s2 = 0.0;
  float r2 = 0.0;
  float a3 = 0.0;
  float d3 = 0.0;
  float s3 = 0.0;
  float r3 = 0.0;

  float wf1 = 0.0;
  float wf2 = 0.0;
  float wf3 = 0.0;

  float cutoff = 0.0;
  float filt1 = 0.0;
  float filt2 = 0.0;
  float filt3 = 0.0;
  float filtExt = 0.0;
  float reso = 0.0;

  float filter_mode = 0.0;
  float master_volume = 0.0;
  
  uint8_t sid[NUM_SID_REGISTERS];
  uint8_t out[SYSEX_DUMP_LENGTH];
  size_t out_data_start = 2;
  uint8_t mod7masks[7] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40};

  SidExConverter() {
    for(int i=0; i<NUM_SID_REGISTERS; i++) {
      sid[i] = 0;
    }
    
    for(size_t i=0; i<SYSEX_DUMP_LENGTH; i++) {
      out[i] = 0;
    }
    out[0] = 0xF0; // sysex start
    out[1] = 0x44; // manufacturer Casio
    out[SYSEX_DUMP_LENGTH-4] = 0x00; // padding
    out[SYSEX_DUMP_LENGTH-3] = 0x00; // padding
    out[SYSEX_DUMP_LENGTH-2] = 0x00; // padding
    out[SYSEX_DUMP_LENGTH-1] = 0xF7; // sysex end
  }

  uint8_t msb(int b) {
    uint8_t retval = (b & 0xFF00) >> 8;
    return retval;
  }
  
  uint8_t lsb(int b) {
    uint8_t retval = b & 0xFF;
    return retval;
  }

  void update_bit(SidRegister reg, float input, uint8_t on_mask) {
    // we have different thresholds for setting and unsetting, so between
    // them we don't change the gate state even if there is some noise
    // TODO Correct value standard?
    if(input >= 2.0) {
      // gate on
      sid[reg] |= on_mask;
    } else if(input <= 1.0) {
      // gate off
      sid[reg] &= (~on_mask);
    }
    // else keep gate as is
  }

  void update_adsr(SidRegister ad, SidRegister sr, float a, float d, float s, float r) {
    uint8_t av = as_nybble(a);
    uint8_t dv = as_nybble(d);
    uint8_t sv = as_nybble(s);
    uint8_t rv = as_nybble(r);

    sid[ad] = av << 4 | dv;
    sid[sr] = sv << 4 | rv;
  }

  uint8_t as_nybble(float v) {
    // the bitmask in the end is maybe paranoia...
    uint8_t n = (v / 10.0) * 15.0;
    return n & 0x0F;
  }

  void update_wf(SidRegister cr, float input) {
    uint8_t value = input * 10.0;
    value = value & 0x0F;
    uint8_t regValue = (value << 4) | (sid[cr] & 0x0F);
    sid[cr] = regValue;
  }

  void update_cutoff() {
    float f = cutoff;
    if(f < 0 ) f = 0;
    int value = (f / 10.0) * 2047.0;
    uint8_t high = (value >> 3) & 0xFF;
    uint8_t low = value & 0x07;
    sid[FcHi] = high;
    sid[FcLo] = low;
  }

  void update_resfilt() {
    update_bit(ResFilt, filt1, 0x01);
    update_bit(ResFilt, filt2, 0x02);
    update_bit(ResFilt, filt3, 0x04);
    update_bit(ResFilt, filtExt, 0x08);

    float resoF = reso;
    if(resoF < 0) resoF = 0.0;
    uint8_t resoValue = (resoF / 10.0) * 15.0;
    sid[ResFilt] |= (resoValue << 4) & 0xF0;
  }
  
  void update_modevol() {
    uint8_t vol = as_nybble(master_volume);
    uint8_t mode = (filter_mode * 10.0);
    mode = (mode << 4) & 0xF0;
    sid[ModeVol] = mode | vol;
  }
  
  void convert() {
    update_bit(CR1, gate1, 0x01);
    update_bit(CR2, gate2, 0x01);
    update_bit(CR3, gate3, 0x01);

    update_adsr(AD1, SR1, a1, d1, s1, r1);
    update_adsr(AD2, SR2, a2, d2, s2, r2);
    update_adsr(AD3, SR3, a3, d3, s3, r3);
    
    int freq1_i = freqInt(freq1);
    sid[FreqHi1] = msb(freq1_i);
    sid[FreqLo1] = lsb(freq1_i);
    int freq2_i = freqInt(freq2);
    sid[FreqHi2] = msb(freq2_i);
    sid[FreqLo2] = lsb(freq2_i);
    int freq3_i = freqInt(freq3);
    sid[FreqHi3] = msb(freq3_i);
    sid[FreqLo3] = lsb(freq3_i);

    update_wf(CR1, wf1);
    update_wf(CR2, wf2);
    update_wf(CR3, wf3);

    update_cutoff();
    update_resfilt();
    
    update_modevol();
    
    // sysex encoding:
    int out_index = out_data_start;
    uint8_t high_bits = 0;
    for(size_t i=0;i < NUM_SID_REGISTERS; i++) {
      size_t mod7 = i % 7;
      uint8_t reg_value = sid[i];
      out[out_index++] = reg_value & 0x7F;
      if((reg_value & 0x80) != 0) {
	high_bits = high_bits | mod7masks[mod7];
      }
      if(mod7 == 6) {
	// end of group
	out[out_index++] = high_bits;
	high_bits = 0;
      }
    }
  }
  
};
