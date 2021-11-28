#!/usr/bin/env python3
import sys
import re

# YOUR CODE GOES HERE

# some global variables
info = {}
# add or mod
add_mod_p = re.compile(r'(add|mod)\s"([a-zA-Z]+\s)*([a-zA-Z]+)"(\s\(-?\d+,-?\d+\))+')
# rm
rm_p = re.compile(r'rm\s"([a-zA-Z]+\s)*([a-zA-Z]+)"')
# gg
gg_p = re.compile(r'gg')


def main():
    # YOUR MAIN CODE GOES HERE

    # sample code to read from stdin.
    # make sure to remove all spurious print statements as required
    # by the assignment
    while True:
        line = sys.stdin.readline()
        if line == "":
            break
        # MODIFY1: delete readline prompt statement
        parse_command(line)

    # MODIFY2: delete finishing prompt statement
    # return exit code 0 on successful termination
    sys.exit(0)


def parse_command(cmd):
    res = re.search(add_mod_p, cmd)
    if res is not None and res.span()[0] == 0 and res.span()[1] == len(cmd) - 1:
        idx1 = cmd.find('"')
        idx2 = cmd.find('"', idx1 + 1)
        str_name = cmd[idx1 + 1: idx2].lower()
        tplist = list()
        tp_list_str = cmd[idx2 + 2:-1].split(' ')
        for item in tp_list_str:
            tplist.append(eval(item))
        if cmd[0] == 'a':
            add(str_name, tplist)
        if cmd[0] == 'm':
            mod(str_name, tplist)
        return
    res = re.search(rm_p, cmd)
    if res is not None and res.span()[0] == 0 and res.span()[1] == len(cmd) - 1:
        idx1 = cmd.find('"')
        idx2 = cmd.find('"', idx1 + 1)
        str_name = cmd[idx1 + 1: idx2].lower()
        rm(str_name)
        return
    res = re.search(gg_p, cmd)
    if res is not None and res.span()[0] == 0 and res.span()[1] == len(cmd) - 1:
        gg()
        return
    # MODIFY3: modify the format of error message
    print("Error: Incorrect input format")


def add(name, tplist):
    if name in info:
        # MODIFY4: modify the format of error message
        print("Error: street currently exists.")
        return
    if len(tplist) == 1:
        # MODIFY5: modify the format of error message
        print("Error: add at least one pair of coordinates.")
        return
    str_info = list()
    for i in range(0, len(tplist) - 1):
        seg = [tplist[i], tplist[i + 1]]
        inter = list()
        isvertex = [False, False]
        seg_info = list()
        seg_info.append(seg)
        seg_info.append(inter)
        seg_info.append(isvertex)
        str_info.append(seg_info)
    info[name] = str_info


def mod(name, tplist):
    if name not in info:
        # MODIFY6: modify the format of error message
        print("Error: 'mod' or 'rm' specified for a street that does not exist.")
        return
    if len(tplist) == 1:
        # MODIFY7: modify the format of error message
        print("Error: specify at least one pair of coordinates.")
        return
    str_info = list()
    for i in range(0, len(tplist) - 1):
        seg = [tplist[i], tplist[i + 1]]
        inter = list()
        isvertex = [False, False]
        seg_info = list()
        seg_info.append(seg)
        seg_info.append(inter)
        seg_info.append(isvertex)
        str_info.append(seg_info)
    info[name] = str_info


def rm(name):
    if name not in info:
        # MODIFY8: modify the format of error message
        print("Error: 'mod' or 'rm' specified for a street that does not exist.")
        return
    del info[name]


def gg():
    resetInfo()
    get_all_intersection()
    isvertex_map = {}
    process_segment_info(isvertex_map)
    sum = generate_snum_for_vertex(isvertex_map)
    initial_list = create_adj_list(sum)
    adj_list = create_edges(isvertex_map, initial_list)
    output_to_a2(adj_list, isvertex_map)


def resetInfo():
    for str_name in info:
        str_info = info[str_name]
        for seg_info in str_info:
            seg_info[1] = list()
            isvertex = seg_info[2]
            isvertex[0] = False
            isvertex[1] = False


# at the begin, info(i.e. street info) store information
# about name of every steet and its segment
def get_all_intersection():
    # get list of street names
    name_list = list(info.keys())
    # calculate all intersections on street i
    for i in range(len(name_list)):
        sti = info[name_list[i]]
        for j in range(i + 1, len(name_list)):
            stj = info[name_list[j]]
            for seg_infoi in sti:
                segi = seg_infoi[0]
                for seg_infoj in stj:
                    # get original seg information
                    segj = seg_infoj[0]
                    # get tuple out of list
                    inter = calculate_intersection(segi, segj)
                    if inter != None:
                        seg_infoi[1].append(inter)
                        seg_infoj[1].append(inter)


# init isvertex_map
# original segment info contains four parts:
# 1.seg,2.intersection list,3.isvertex list indicating whether seg[0] or seg[1] is a vertex
def process_segment_info(isvertex_map):
    for str_name in info:
        str_info = info[str_name]
        for seg_info in str_info:
            seg = seg_info[0]
            inter = seg_info[1]
            isvertex = seg_info[2]
            isvertex[0] = len(inter) > 0
            isvertex[1] = len(inter) > 0
            for v in inter:
                if v not in isvertex_map:
                    isvertex_map[v] = [True, 0]
            if seg[0] not in isvertex_map:
                isvertex_map[seg[0]] = [False, 0]
            if seg[1] not in isvertex_map:
                isvertex_map[seg[1]] = [False, 0]
            if isvertex[0]:
                isvertex_map[seg[0]][0] = True
                isvertex_map[seg[1]][0] = True


def generate_snum_for_vertex(isvertex_map):
    snum = 1
    for p in isvertex_map:
        if isvertex_map[p][0]:
            isvertex_map[p][1] = snum
            snum = snum + 1
    return snum


def create_adj_list(snum):
    adj_list = list()
    for i in range(snum + 1):
        adj_list.append(set())
    return adj_list


def create_edges(isvertex_map, adj_list):
    for str_name in info:
        str_info = info[str_name]
        for seg_info in str_info:
            seg = seg_info[0]
            inter = seg_info[1]
            vlist = list()
            if seg[0] not in inter and isvertex_map[seg[0]][0]:
                vlist.append(seg[0])
            if seg[1] not in inter and isvertex_map[seg[1]][0]:
                vlist.append(seg[1])
            for it in inter:
                vlist.append(it)
            if len(vlist) > 1:
                # sort tuple according to their x position from left to right
                vlist.sort()
                # begin create edges
                for i in range(len(vlist) - 1):
                    cur = vlist[i]
                    next = vlist[i + 1]
                    if cur in inter or next in inter:
                        cur_snum = isvertex_map[cur][1]
                        next_snum = isvertex_map[next][1]
                        adj_list[cur_snum].add(next_snum)
                        adj_list[next_snum].add(cur_snum)
    return adj_list


def show_graph(adj_list, isvertex_map):
    pmap = {}
    # show vertices
    print("V = {")
    for v in isvertex_map:
        if isvertex_map[v][0]:
            pmap[isvertex_map[v][1]] = v
            # MODIFY9: show two decimal places
            print(str(isvertex_map[v][1]) + ":  (" + ("%.2f" % v[0]) + "," + ("%.2f" % v[1]) + ")")
    print("}")

    n = len(adj_list)
    flag_table = []
    for i in range(n):
        flag_table.append(list())
        for j in range(n):
            flag_table[i].append(False)
    # show edges
    # MODIFY10: firstly add edges to a list(doesn't change what edges to output), then print these edges with a comma
    # if it is not the last one. Otherwise print the last edge without a comma
    edgelist = list()
    for i in range(1, len(adj_list)):
        for neighbor_snum in adj_list[i]:
            if i != neighbor_snum and not flag_table[i][neighbor_snum]:
                edgelist.append(str("<" + str(i) + "," + str(neighbor_snum) + ">"))
                flag_table[i][neighbor_snum] = True
                flag_table[neighbor_snum][i] = True
    print("E = {")
    for i in range(len(edgelist)):
        if i < len(edgelist)-1:
            print (edgelist[i] + ",")
        else:
            print (edgelist[i])
    print("}")


def output_to_a2(adj_list, isvertex_map):
    vertex_map = {}
    count = 0
    pmap = {}
    # show vertices
    for v in isvertex_map:
        if isvertex_map[v][0]:
            pmap[isvertex_map[v][1]] = v
            # MODIFY9: show two decimal places
            count = count + 1
            vertex_map[str(isvertex_map[v][1])] = str(count)
            # print(str(isvertex_map[v][1]) + ":  (" + ("%.2f" % v[0]) + "," + ("%.2f" % v[1]) + ")")

    n = len(adj_list)
    flag_table = []
    for i in range(n):
        flag_table.append(list())
        for j in range(n):
            flag_table[i].append(False)
    # show edges
    # MODIFY10: firstly add edges to a list(doesn't change what edges to output), then print these edges with a comma
    # if it is not the last one. Otherwise print the last edge without a comma
    edgelist = list()
    for i in range(1, len(adj_list)):
        for neighbor_snum in adj_list[i]:
            if i != neighbor_snum and not flag_table[i][neighbor_snum]:
                edgelist.append(str("<" + vertex_map[str(i)] + "," + vertex_map[str(neighbor_snum)] + ">"))
                flag_table[i][neighbor_snum] = True
                flag_table[neighbor_snum][i] = True

    v_str = "V " + str(count)
    e_str = "E {"
    for i in range(len(edgelist)):
        e_str += edgelist[i]
        if i != len(edgelist)-1:
            e_str += ","
    e_str += "}"
    print(v_str)
    print(e_str)


def calculate_intersection(l1, l2):
    p1 = l1[0]
    p2 = l1[1]
    p3 = l2[0]
    p4 = l2[1]
    x1 = p1[0]
    y1 = p1[1]
    x2 = p2[0]
    y2 = p2[1]
    x3 = p3[0]
    y3 = p3[1]
    x4 = p4[0]
    y4 = p4[1]
    # two lines are vertical
    if x1 == x2 and x3 == x4:
        return None
    # l1 is vertical and l2 is horizontal
    if x1 == x2:
        k2 = (y4 - y3) / (x4 - x3)
        x = x1
        y = k2 * (x - x3) + y3
        if (x - x1) * (x - x2) <= 0 and (x - x3) * (x - x4) <= 0 and (y - y1) * (y - y2) <= 0 and (y - y3) * (
                y - y4) <= 0:
            return (round(x, 3), round(y, 3))
        else:
            return None
    # l1 is horizontal and l2 is vertical
    if x3 == x4:
        k1 = (y2 - y1) / (x2 - x1)
        x = x3
        y = k1 * (x - x1) + y1
        if (x - x1) * (x - x2) <= 0 and (x - x3) * (x - x4) <= 0 and (y - y1) * (y - y2) <= 0 and (y - y3) * (
                y - y4) <= 0:
            return (round(x, 3), round(y, 3))
        else:
            return None
            # both l1 and l2 are not vertical
    k1 = (y2 - y1) / (x2 - x1)
    k2 = (y4 - y3) / (x4 - x3)
    # l1 and l2 are parallel
    if k1 == k2:
        if p1 == p3 or p1 == p4:
            return (x1, y1)
        elif p2 == p3 or p2 == p4:
            return (x2, y2)
        else:
            return None
    x = (y3 - y1 + k1 * x1 - k2 * x3) / (k1 - k2)
    y = k1 * (x - x1) + y1
    if (x - x1) * (x - x2) <= 0 and (x - x3) * (x - x4) <= 0 and (y - y1) * (y - y2) <= 0 and (y - y3) * (y - y4) <= 0:
        return (round(x, 3), round(y, 3))
    else:
        return None


if __name__ == "__main__":
    main()
