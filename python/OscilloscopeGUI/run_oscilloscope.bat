set PYTHONPATH=%PYTHONPATH%;./script
python ./script/oscilloscope.py --port COM6 --oscilloscope_mode --show_grid --right_bit_shift 3 --record_time 8.0 --record_folder record
#python ./script/oscilloscope.py --port COM6 --oscilloscope_mode