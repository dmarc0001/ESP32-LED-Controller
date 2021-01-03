#!/usr/bin/python3
# -*- coding: utf-8 -*-

from argparse import ArgumentParser
from os import path, remove
import gzip
import shutil

"""
Versionsnummer hochzählen für UPDATE
"""
__author__ = 'Dirk Marciniak'
__copyright__ = 'Copyright 2020'
__license__ = 'GPL'
__version__ = '0.2'

"""
 elegentWebpage.h 
 const uint8_t ELEGANT_HTML[] PROGMEM = { XXXX } in Datei extraieren
 python main.py --decode --infile extraierte_datei.txt --outfile gezipptes_archiv.gz

 Archiv entzippen, html bearbeiten, neue zip datei anlegen

 python main.py --encode --infile neue_datei.gz --outfile new_datdump.txt

 die datei new_datdump.txt ind die headerdatei einbringen

 neu compiliereen

"""


def decode_to_source(infile, outfile):
    counter = 0
    compressed_file = 'temp_' + infile + '.gz';
    print("decode hexdump file <{0}> to source file <{1}>".format(infile, outfile))
    #
    # zuest hex zu binärformat (gzip file)
    #
    try:
        print("open hexfile {}...".format(infile))
        i_file = open(infile, 'r')
    except IOError as e:
        print("can't open hexfile. abort : {}".format(e))
        return (-1)
    try:
        print("open gzip tempfile {}...".format(compressed_file))
        o_file = open(compressed_file, 'wb')
    except IOError as e:
        i_file.close()
        print("can't open gzip tempfile. abort : {}".format(e))
        return (-1)
    try:
        for line in i_file:
            tokenlist = line.split(',')
            for token in tokenlist:
                val = token.strip()
                if len(val) > 0:
                    num = int(val, base=10).to_bytes(1, 'little')
                    # byte_l = num.to_bytes(1,'little')
                    o_file.write(num)
                    # print("Token {0:06d}: 0x{1}".format(counter, num))
                    counter += 1
        i_file.close()
        o_file.close()
        print("create gzip tempfile ok...")
    except IOError as e:
        i_file.close()
        o_file.close()
        print("can't write gzip tempfile. abort : {}".format(e))
        return (-1)
    #
    # jetzt zum sourcefile dekomprimieren
    #
    try:
        print("decompress file to source file...")
        with gzip.open(compressed_file, 'rb') as f_in:
            with open(outfile, 'wb') as f_out:
                shutil.copyfileobj(f_in, f_out)
        print("decompress file to source file...OK")
    except IOError as e:
        print("can't decompress infile. abort : {}".format(e))
        return (-1)
    remove(compressed_file)


def encode_to_hexdump(infile, outfile):
    counter = 0
    compressed_file = 'temp_' + infile + '.gz';
    print("encode ascii source file <{0}> to hexdump file <{1}>".format(infile, outfile))
    #
    # erst komprimieren
    #
    try:
        with open(infile, 'rb') as f_in:
            with gzip.open(compressed_file, 'wb') as f_out:
                shutil.copyfileobj(f_in, f_out)
    except IOError as e:
        print("can't compress infile. abort : {}".format(e))
        return (-1)
    #
    # jetzt hexdump erzeugen
    #
    try:
        print("open compressed infile {}...".format(compressed_file))
        i_file = open(compressed_file, 'rb')
    except IOError as e:
        print("can't open infile. abort : {}".format(e))
        return (-1)
    try:
        print("open hexdump outfile {}...".format(outfile))
        o_file = open(outfile, 'w')
    except IOError as e:
        i_file.close()
        print("can't open hexdump outfile. abort : {}".format(e))
        remove(compressed_file)
        return (-1)
    rbyte = i_file.read(1)
    while rbyte:
        #
        # wandeln nach code
        #
        num = int.from_bytes(rbyte, 'little', signed=False)
        ascii_str = "{0:d}".format(num)
        # print("char: {0}".format(ascii_str))
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
    file_size = path.getsize(compressed_file)
    remove(compressed_file)
    print("compressed file size: {} bytes".format(file_size))

def main():
    infile = None
    outfile = "output.file"
    is_decode = False
    is_encode = False
    use_gzip = False
    #
    # parse argumente
    #
    parser = ArgumentParser(fromfile_prefix_chars='@',
                            description='decode/code for c program',
                            epilog="from Dirk Marciniak\n");
    parser.add_argument("--infile", type=str, help="input file")
    parser.add_argument("--outfile", type=str, help="output file")
    parser.add_argument("--decode", default=False, action='store_true', help="decode hexdump file to source file")
    parser.add_argument("--encode", default=False, action='store_true', help="encode source file to hexdump file")
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
            outfile = infile + ".txt"
        decode_to_source(infile, outfile)
    else:
        if args.outfile:
            outfile = args.outfile
        else:
            outfile = infile + ".hex"
        if is_encode:
            encode_to_hexdump(infile, outfile)
        else:
            print("none encode or decode... abort")
            exit(-1)


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    main()


