import os
import sys

def write_block(cpp, beg_patt, end_patt, expr, seq):
    with open(cpp) as src:
        content = src.read()

    pos_beg = content.find(beg_patt)
    pos_end = content.find(end_patt)

    if pos_beg == -1 or pos_beg == -1:
        return
    pos_beg = pos_beg + len(beg_patt)

    content = ( content[:pos_beg] + " \\\n"
                + "/* " + expr + " */ \\\n"
                + seq + "\n"
                + content[pos_end:]
            )

    with open(cpp, "w") as src:
        src.write(content)

def sub_op(m):
    NUM_CHAR_PER_LINE = 10

    n = NUM_CHAR_PER_LINE
    if (m.start() + 1) % (n + 2 * n) == 0:
        return "', \\\n'"
    return "', '"


CPPDIR = "E:\\my_code\\learning\\C++"
os.chdir(CPPDIR)

if len(sys.argv) < 3:
    print("arguments < 3!")
    exit()

if sys.argv[1] == "run":

    if len(sys.argv) < 4:
        print("cadriv-run: arguments < 4!")
        exit()

    with open(sys.argv[2]) as ei:
        expr = ei.readline()[:-1]

    if len(expr) < 1:
        print("expression input error!")
        exit()

    import re
    re_char = re.compile("(.)")

    chars = re.sub(re_char, lambda m: "'" + m.group(1) + "'", expr)
    #print(chars)

    seq = re.sub("''", sub_op, chars)
    #print(seq)

    write_block(sys.argv[3], "/*{{{*/", "/*}}}*/", expr, seq)

elif sys.argv[1] == "res":
    
    with open(sys.argv[2]) as f:
        dmp = f.read()

    STR_TYPE = "XXXXXXXXXXXXXXXXXXXX__deduced_type_is_<";
    pos = dmp.find(STR_TYPE)
    if pos == -1:
        print("dmp contents error")
        exit()
    pos = pos + len(STR_TYPE)

    import re
    res_mch = re.search("ratio<\s*(-?\d+)\s*,\s*(-?\d+)\s*>", dmp[pos:])
    print("\n\n == %f" % (float(res_mch.group(1)) / float(res_mch.group(2))))

else:
    print("usage: cadriv (\"run\"|\"res\") <input-file-path>")
