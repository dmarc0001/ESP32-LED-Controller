#!/usr/bin/python3
# -*- coding: utf-8 -*-

from argparse import ArgumentParser
import datetime
from os import path, remove, rename
import platform
import re
import array as arr

"""
Versionsnummer hochzählen für UPDATE
"""
__author__ = 'Dirk Marciniak'
__copyright__ = 'Copyright 2020'
__license__ = 'GPL'
__version__ = '0.1'


"""
 elegentWebpage.h 
 const uint8_t ELEGANT_HTML[] PROGMEM = { XXXX } in Datei extraieren
 python main.py --decode --infile extraierte_datei.txt --outfile gezipptes_archiv.gz
 
 Archiv entzippen, html bearbeiten, neue zip datei anlegen
 
 python main.py --encode --infile neue_datei.gz --outfile new_datdump.txt
 
 die datei new_datdump.txt ind die headerdatei einbringen
 
 neu compiliereen

"""




def decode_to_binary(infile, outfile):
    counter = 0
    print("decode ascii file <{0}> to binary file <{1}>".format(infile, outfile))
    try:
        print("open infile {}...".format(infile))
        i_file = open(infile, 'r')
    except IOError as e:
        print("can't open infile. abort : {}".format(e))
        return(-1)
    try:
        print("open binary outfile {}...".format(outfile))
        o_file = open(outfile, 'wb')
    except IOError as e:
        i_file.close()
        print("can't open binary outfile. abort : {}".format(e))
        return(-1)

    try:
        for line in i_file:
            tokenlist = line.split(',')
            for token in tokenlist:
                val = token.strip()
                if len(val) > 0:
                    num = int(val, base=10).to_bytes(1,'little')
                    #byte_l = num.to_bytes(1,'little')
                    o_file.write(num)
                    #print("Token {0:06d}: 0x{1}".format(counter, num))
                    counter += 1
        i_file.close()
        o_file.close()
        print("decoding done...")
    except IOError as e:
        i_file.close()
        o_file.close()
        print("can't write outfile. abort : {}".format(e))
        return(-1)
    pass


def encode_to_ascii(infile, outfile):
    print("encode binary file <{0}> to ascii file <{1}>".format(infile, outfile))
    counter = 0
    print("decode ascii file <{0}> to binary file <{1}>".format(infile, outfile))
    try:
        print("open infile {}...".format(infile))
        i_file = open(infile, 'rb')
    except IOError as e:
        print("can't open infile. abort : {}".format(e))
        return(-1)
    try:
        print("open binary outfile {}...".format(outfile))
        o_file = open(outfile, 'w')
    except IOError as e:
        i_file.close()
        print("can't open binary outfile. abort : {}".format(e))
        return(-1)
    rbyte = i_file.read(1)
    while rbyte:
        #
        # wandeln nach code
        #
        num = int.from_bytes(rbyte, 'little', signed=False)
        ascii_str = "{0:d}".format(num)
        #print("char: {0}".format(ascii_str))
        #
        # versuchen das nächste Byte zu holen
        #
        rbyte = i_file.read(1)
        if rbyte:
            ascii_str += ","
        o_file.write(ascii_str)
        counter += 1
        if counter >= 29:
            counter = 0
            o_file.write("\n")
    o_file.close()
    i_file.close()

def main():
    infile = None
    outfile = "output.file"
    is_decode = False
    is_encode = False
    #
    # parse argumente
    #
    parser = ArgumentParser(fromfile_prefix_chars='@',
                            description='decode/code for c program',
                            epilog="from Dirk Marciniak\n");
    parser.add_argument("--infile", type=str, help="input file")
    parser.add_argument("--outfile", type=str, help="output file")
    parser.add_argument("--decode", default=False, action='store_true', help="decode text to binary")
    parser.add_argument("--encode", default=False, action='store_true', help="encode binary to text")
    #
    args = parser.parse_args()
    #
    # eingabedatei
    #
    if args.infile:
        infile = args.infile
        if path.isfile(infile):
            print("infile is a file and his name is {}".format(infile))
        else:
            print('infile {} is not exist. abort.'.format(infile))
            exit(-1)
    else:
        print("there is no infile, abort!")
        exit(-1)
    #
    # decode oder encode
    #
    if args.encode:
        is_encode = True
    if args.decode:
        is_decode = True
    #
    # jetzt geht es los
    #
    if is_decode:
        if args.outfile:
            outfile = args.outfile
        else:
            outfile = infile + ".gz"
        decode_to_binary(infile, outfile)
    else:
        if args.outfile:
            outfile = args.outfile
        else:
            outfile = infile + ".txt"
        if is_encode:
            encode_to_ascii(infile, outfile)
        else:
            print("none encode or decode... abort")
            exit(-1)

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()


