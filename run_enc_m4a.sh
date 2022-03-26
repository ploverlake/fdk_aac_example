#!/bin/bash
PROG_DIR="$(cd -- "$(dirname "$0")" >/dev/null 2>&1 && pwd -P)"
PROG_NAME="$(basename "$0")"

ENC_PROG="$PROG_DIR/example/aac_m4a_enc"
if [ ! -x "$ENC_PROG" ]; then
  echo "Please build aac_m4a_enc first ..."
  exit 1
fi

print_usage() {
  echo "Usage: $PROG_NAME [options] /path/to/wav_file"
  echo "Encode aac to a m4a file"
  echo "  -h, --help  Display this usage and then exit"
  echo "  -a, --aot   AOT of aac, default is '2'(LC)."
  echo "              Supported AOTs are '2(LC)', '5(HE)', '29(HEv2)', '23(LD)', '39(ELD)'"
}

wav_file=
aac_aot="2"
while [ $# -gt 0 ]; do
  case "$1" in
    -h | --help)
      print_usage
      exit 0
      ;;
    -a | --aot)
      shift
      aac_aot="$1"
      ;;
    -*)
      echo "Warning: unknown option($1)"
      ;;
    *)
      wav_file="$1"
      ;;
  esac
  shift
done

if [ ! -f "$wav_file" ]; then
  echo "Error: '$wav_file' does not exist"
  exit 1
fi

aac_aot_name="unknown"
if [ "$aac_aot" = "2" ]; then
  aac_aot_name="LC"
elif [ "$aac_aot" = "5" ]; then
  aac_aot_name="HE"
elif [ "$aac_aot" = "29" ]; then
  aac_aot_name="HEv2"
elif [ "$aac_aot" = "23" ]; then
  aac_aot_name="LD"
elif [ "$aac_aot" = "39" ]; then
  aac_aot_name="ELD"
else
  echo "Error: unsupported AOT('$aac_aot')"
  exit 1
fi

wav_file_dir="$(cd -- "$(dirname "$wav_file")" >/dev/null 2>&1 && pwd -P)"
wav_file_name="$(basename "$wav_file")"
wav_file="$wav_file_dir/$wav_file_name"

bitrate_list=(24000 64000 96000 128000)

echo "Wav file: $wav_file"
echo "AOT: $aac_aot"
echo "Bit rate list: ${bitrate_list[*]}"

for encode_bitrate in "${bitrate_list[@]}"; do
  echo ""
  enc_cmd="$ENC_PROG -t $aac_aot -r $encode_bitrate $wav_file $wav_file_dir/${wav_file_name}.${aac_aot_name}.${encode_bitrate}.m4a"
  echo "$enc_cmd"
  eval "$enc_cmd"
done
