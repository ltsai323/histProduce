#!/usr/bin/env python2

template='''
#ifndef __TriggerBooking_h__
#define __TriggerBooking_h__
#include <string>
namespace HLTlist
{{
    const std::string trigName[] = {{
        {0}
    }};

    enum trigNum {{
        {1}
    }};
}}
#endif
'''

def HLTlist():
    lists=[]
    for line in open('../data/triggerLIST.asc'):
        l = line.strip()
        for hlt in l.split(' '):
            if not hlt.startswith('#'):
                lists.append(hlt)
    return lists
def changeToCPPstr(nameList):
    return [ '"{}*"'.format(name) for name in nameList ]
def changeToCPPenum(nameList):
    return [ '{0}={1}'.format(nameList[i], i) for i in xrange(len(nameList)) ]


if __name__ == '__main__':
    origHLT = HLTlist()
    name1 = changeToCPPstr(origHLT)
    name2 = changeToCPPenum(origHLT)
    name2.append('totNum={}'.format(len(name2)))
    tempRes = template.format(',\n\t\t'.join(name1),',\n\t\t'.join(name2))
    with open('../interface/TriggerBooking.h', mode='w') as outfile:
        outfile.write(tempRes)

