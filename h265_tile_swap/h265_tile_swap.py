from  bitstring import BitStream

class NalUnitType:
    """
    Table 7-1 - NAL unit type codes and NAL unit type classes
    copypaste from source/Lib/TLibCommon/CommonDef.h
    """
    NAL_UNIT_CODED_SLICE_TRAIL_N = 0
    NAL_UNIT_CODED_SLICE_TRAIL_R = 1

    NAL_UNIT_CODED_SLICE_TSA_N = 2
    NAL_UNIT_CODED_SLICE_TSA_R = 3

    NAL_UNIT_CODED_SLICE_STSA_N = 4
    NAL_UNIT_CODED_SLICE_STSA_R = 5

    NAL_UNIT_CODED_SLICE_RADL_N = 6
    NAL_UNIT_CODED_SLICE_RADL_R = 7

    NAL_UNIT_CODED_SLICE_RASL_N = 8
    NAL_UNIT_CODED_SLICE_RASL_R = 9

    NAL_UNIT_RESERVED_VCL_N10 = 10
    NAL_UNIT_RESERVED_VCL_R11 = 11
    NAL_UNIT_RESERVED_VCL_N12 = 12
    NAL_UNIT_RESERVED_VCL_R13 = 13
    NAL_UNIT_RESERVED_VCL_N14 = 14
    NAL_UNIT_RESERVED_VCL_R15 = 15

    NAL_UNIT_CODED_SLICE_BLA_W_LP = 16
    NAL_UNIT_CODED_SLICE_BLA_W_RADL = 17
    NAL_UNIT_CODED_SLICE_BLA_N_LP = 18
    NAL_UNIT_CODED_SLICE_IDR_W_RADL = 19
    NAL_UNIT_CODED_SLICE_IDR_N_LP = 20
    NAL_UNIT_CODED_SLICE_CRA = 21
    NAL_UNIT_RESERVED_IRAP_VCL22 = 22
    NAL_UNIT_RESERVED_IRAP_VCL23 = 23

    NAL_UNIT_RESERVED_VCL24 = 24
    NAL_UNIT_RESERVED_VCL25 = 25
    NAL_UNIT_RESERVED_VCL26 = 26
    NAL_UNIT_RESERVED_VCL27 = 27
    NAL_UNIT_RESERVED_VCL28 = 28
    NAL_UNIT_RESERVED_VCL29 = 29
    NAL_UNIT_RESERVED_VCL30 = 30
    NAL_UNIT_RESERVED_VCL31 = 31

    NAL_UNIT_VPS = 32
    NAL_UNIT_SPS = 33
    NAL_UNIT_PPS = 34
    NAL_UNIT_ACCESS_UNIT_DELIMITER = 35
    NAL_UNIT_EOS = 36
    NAL_UNIT_EOB = 37
    NAL_UNIT_FILLER_DATA = 38
    NAL_UNIT_PREFIX_SEI = 39
    NAL_UNIT_SUFFIX_SEI = 40

    NAL_UNIT_RESERVED_NVCL41 = 41
    NAL_UNIT_RESERVED_NVCL42 = 42
    NAL_UNIT_RESERVED_NVCL43 = 43
    NAL_UNIT_RESERVED_NVCL44 = 44
    NAL_UNIT_RESERVED_NVCL45 = 45
    NAL_UNIT_RESERVED_NVCL46 = 46
    NAL_UNIT_RESERVED_NVCL47 = 47
    NAL_UNIT_UNSPECIFIED_48 = 48
    NAL_UNIT_UNSPECIFIED_49 = 49
    NAL_UNIT_UNSPECIFIED_50 = 50
    NAL_UNIT_UNSPECIFIED_51 = 51
    NAL_UNIT_UNSPECIFIED_52 = 52
    NAL_UNIT_UNSPECIFIED_53 = 53
    NAL_UNIT_UNSPECIFIED_54 = 54
    NAL_UNIT_UNSPECIFIED_55 = 55
    NAL_UNIT_UNSPECIFIED_56 = 56
    NAL_UNIT_UNSPECIFIED_57 = 57
    NAL_UNIT_UNSPECIFIED_58 = 58
    NAL_UNIT_UNSPECIFIED_59 = 59
    NAL_UNIT_UNSPECIFIED_60 = 60
    NAL_UNIT_UNSPECIFIED_61 = 61
    NAL_UNIT_UNSPECIFIED_62 = 62
    NAL_UNIT_UNSPECIFIED_63 = 63
    NAL_UNIT_INVALID = 64


class NalExtractor:
    def __init__(self, inpFile: str):
        self.inp_fd = open(inpFile, "rb")
        self.curWindow = self.inp_fd.read(4096)
        self.curPos = 0
        self.s = BitStream(self.curWindow)
        while self.s.peek('hex:24') != '000001':
            self.s.read(8)

    def _updateCurWindow(self):
        bytes_remaining = len(self.curWindow) - self.curPos
        self.curWindow = bytearray(self.curWindow[-bytes_remaining:])
        nextBytes = self.inp_fd.read(self.s.len - bytes_remaining)
        self.curWindow.extend(nextBytes)
        self.curPos = 0
        if len(nextBytes) == 0:
            retval = 1
        else:
            retval = 0
        return retval

    def peekStartCode(self):
        p = self.curPos
        ret_val = (self.curWindow[p] == 0x0 and
                   self.curWindow[p+1] == 0x0 and
                   self.curWindow[p+2] == 0x1)
        return ret_val

    def getNextNal(self):
        eof_flag = 0
        nal_buf = bytearray()

        nal_buf.extend(self.curWindow[self.curPos: self.curPos+3])
        self.curPos += 3

        while not self.peekStartCode():
            if (len(self.curWindow) - self.curPos) < 4:
                ret_val = self._updateCurWindow()
                if ret_val != 0:
                    nal_buf.extend(self.curWindow[self.curPos:])
                    print("END of file", self.curPos, len(self.curWindow), len(nal_buf))
                    self.curPos = len(self.curWindow)
                    eof_flag = 1
                    break

            nal_buf.append(self.curWindow[self.curPos])
            self.curPos += 1
        print(len(nal_buf))
        return nal_buf, eof_flag

class NalUnitHeader:
    def __init__(self, nal_buf):
        s = BitStream(nal_buf[3:])
        self.forbidden_zero_bit  = s.read('uint:1')
        self.nal_unit_type = s.read('uint:6')
        self.nuh_layer_id = s.read('uint:6')
        self.nuh_temporal_id_plus1 = s.read('uint:3')


if __name__ == "__main__":
    import argparse
    import sys

    parser = argparse.ArgumentParser(description='H265 tile swap in')
    # Position Argument
    parser.add_argument('-b', "--base_file", help="Base H265 file in which a tile is to be replaced", type=str)

    parser.add_argument('-r', '--replcement', required=True, 
                help='H265 File from which a replacement tile will be extracted', type=str)
    parser.add_argument('-n', '--num_tiles', help='Number of tiles i.e. n*n in nxn tiling', type=int)
    parser.add_argument('-bt', '--base_tile', help='Select base tile to be replaced', type=int)
    parser.add_argument('-rt', '--replace_tile', help='Select replacement tile', type=int)
    parser.add_argument('-o', '--output', help='Output H265 file', type=str)

    options = parser.parse_args()

    base_nal_ext = NalExtractor(options.base_file)
    rep_nal_ext = NalExtractor(options.replcement)
    #base_nal_ext = NalExtractor(BitStream(filename=options.base_file))
    #rep_nal_ext = NalExtractor(BitStream(filename=options.replcement))

    vcl_nal_count = 0

    all_replacement_tiles = []
    while True:
        r_nal, eof_flag = rep_nal_ext.getNextNal()
        nh = NalUnitHeader(r_nal)
        if (nh.nal_unit_type == NalUnitType.NAL_UNIT_CODED_SLICE_TRAIL_R or
            nh.nal_unit_type == NalUnitType.NAL_UNIT_CODED_SLICE_IDR_W_RADL):
            vcl_nal_count += 1

            if vcl_nal_count % options.num_tiles == options.replace_tile:
                all_replacement_tiles.append(r_nal)
                print("ReplacementLen:", len(r_nal))

        if eof_flag == 1:
            break

    print("Replacement nal_count = ", vcl_nal_count)

    #sys.exit(1)
    all_replacement_tiles.reverse()
    vcl_nal_count = 0
    with open(options.output, "wb") as fd:
        while True:
            b_nal, eof_flag  = base_nal_ext.getNextNal()
            
            nh = NalUnitHeader(b_nal)
            print(nh.nal_unit_type)

            if (nh.nal_unit_type == NalUnitType.NAL_UNIT_CODED_SLICE_TRAIL_R or
               nh.nal_unit_type == NalUnitType.NAL_UNIT_CODED_SLICE_IDR_W_RADL):
                vcl_nal_count += 1
                if vcl_nal_count % options.num_tiles != options.base_tile:
                    fd.write(b_nal)
                else:
                    r_nal = all_replacement_tiles.pop()
                    fd.write(r_nal)

                    print("Replacing:", len(r_nal), len(b_nal))
            else:
                fd.write(b_nal)

            if eof_flag == 1:
                break
