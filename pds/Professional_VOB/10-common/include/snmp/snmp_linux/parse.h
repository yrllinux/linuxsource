#ifndef PARSE_H
#define PARSE_H
#include "snmp_linux/snmp_module.h"

#ifdef __cplusplus
extern          "C" {
#endif
    /*
     * parse.h
     */
/***********************************************************
        Copyright 1989 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/

#define MAXLABEL        64      /* maximum characters in a label */
#define MAXTOKEN        128     /* maximum characters in a token */
#define MAXQUOTESTR     4096    /* maximum characters in a quoted string */

    struct variable_list;

    /*
     * A linked list of tag-value pairs for enumerated integers.
     */
    struct enum_list {
        struct enum_list *next;
        int             value;
        char           *label;
    };

    /*
     * A linked list of ranges
     */
    struct range_list {
        struct range_list *next;
        int             low, high;
    };

    /*
     * A linked list of indexes
     */
    struct index_list {
        struct index_list *next;
        char           *ilabel;
        char            isimplied;
    };

    /*
     * A linked list of varbinds
     */
    struct varbind_list {
        struct varbind_list *next;
        char           *vblabel;
    };

    /*
     * A linked list of nodes.
     */
    struct node {
        struct node    *next;
        char           *label;  /* This node's (unique) textual name */
        u_long          subid;  /* This node's integer subidentifier */
        int             modid;  /* The module containing this node */
        char           *parent; /* The parent's textual name */
        int             tc_index;       /* index into tclist (-1 if NA) */
        int             type;   /* The type of object this represents */
        int             access;
        int             status;
        struct enum_list *enums;        /* (optional) list of enumerated integers */
        struct range_list *ranges;
        struct index_list *indexes;
        char           *augments;
        struct varbind_list *varbinds;
        char           *hint;
        char           *units;
        char           *description;    /* description (a quoted string) */
        char           *reference;    /* references (a quoted string) */
        char           *defaultValue;
	char           *filename;
        int             lineno;
    };

    /*
     * A tree in the format of the tree structure of the MIB.
     */
    struct tree {
        struct tree    *child_list;     /* list of children of this node */
        struct tree    *next_peer;      /* Next node in list of peers */
        struct tree    *next;   /* Next node in hashed list of names */
        struct tree    *parent;
        char           *label;  /* This node's textual name */
        u_long          subid;  /* This node's integer subidentifier */
        int             modid;  /* The module containing this node */
        int             number_modules;
        int            *module_list;    /* To handle multiple modules */
        int             tc_index;       /* index into tclist (-1 if NA) */
        int             type;   /* This node's object type */
        int             access; /* This nodes access */
        int             status; /* This nodes status */
        struct enum_list *enums;        /* (optional) list of enumerated integers */
        struct range_list *ranges;
        struct index_list *indexes;
        char           *augments;
        struct varbind_list *varbinds;
        char           *hint;
        char           *units;
        int             (*printomat) (u_char **, size_t *, size_t *, int,
                                      const netsnmp_variable_list *,
                                      const struct enum_list *, const char *,
                                      const char *);
        void            (*printer) (char *, const netsnmp_variable_list *, const struct enum_list *, const char *, const char *);   /* Value printing function */
        char           *description;    /* description (a quoted string) */
        char           *reference;    /* references (a quoted string) */
        int             reported;       /* 1=report started in print_subtree... */
        char           *defaultValue;
    };

    /*
     * Information held about each MIB module
     */
    struct module_import {
        char           *label;  /* The descriptor being imported */
        int             modid;  /* The module imported from */
    };

    struct module {
        char           *name;   /* This module's name */
        char           *file;   /* The file containing the module */
        struct module_import *imports;  /* List of descriptors being imported */
        int             no_imports;     /* The number of such import descriptors */
        /*
         * -1 implies the module hasn't been read in yet 
         */
        int             modid;  /* The index number of this module */
        struct module  *next;   /* Linked list pointer */
    };

    struct module_compatability {
        const char     *old_module;
        const char     *new_module;
        const char     *tag;    /* NULL implies unconditional replacement,
                                 * otherwise node identifier or prefix */
        size_t          tag_len;        /* 0 implies exact match (or unconditional) */
        struct module_compatability *next;      /* linked list */
    };
#ifdef MOD_SNMP_VER_0_1
#define MAXTC   128//objid type num 4096
#define SYNTAX_MASK     0x80
/*
 * types of tokens
 * Tokens wiht the SYNTAX_MASK bit set are syntax tokens 
 */
#define CONTINUE    -1
#define ENDOFFILE   0
#define LABEL       1
#define SUBTREE     2
#define SYNTAX      3
#define OBJID       (4 | SYNTAX_MASK)
#define OCTETSTR    (5 | SYNTAX_MASK)
#define INTEGER     (6 | SYNTAX_MASK)
#define NETADDR     (7 | SYNTAX_MASK)
#define IPADDR      (8 | SYNTAX_MASK)
#define COUNTER     (9 | SYNTAX_MASK)
#define GAUGE       (10 | SYNTAX_MASK)
#define TIMETICKS   (11 | SYNTAX_MASK)
#define KW_OPAQUE   (12 | SYNTAX_MASK)
#define NUL         (13 | SYNTAX_MASK)
#define SEQUENCE    14
#define OF          15          /* SEQUENCE OF */
#define OBJTYPE     16
#define ACCESS      17
#define READONLY    18
#define READWRITE   19
#define WRITEONLY   20
#ifdef NOACCESS
#undef NOACCESS                 /* agent 'NOACCESS' token */
#endif
#define NOACCESS    21
#define STATUS      22
#define MANDATORY   23
#define KW_OPTIONAL    24
#define OBSOLETE    25
/*
 * #define RECOMMENDED 26 
 */
#define PUNCT       27
#define EQUALS      28
#define NUMBER      29
#define LEFTBRACKET 30
#define RIGHTBRACKET 31
#define LEFTPAREN   32
#define RIGHTPAREN  33
#define COMMA       34
#define DESCRIPTION 35
#define QUOTESTRING 36
#define INDEX       37
#define DEFVAL      38
#define DEPRECATED  39
#define SIZE        40
#define BITSTRING   (41 | SYNTAX_MASK)
#define NSAPADDRESS (42 | SYNTAX_MASK)
#define COUNTER64   (43 | SYNTAX_MASK)
#define OBJGROUP    44
#define NOTIFTYPE   45
#define AUGMENTS    46
#define COMPLIANCE  47
#define READCREATE  48
#define UNITS       49
#define REFERENCE   50
#define NUM_ENTRIES 51
#define MODULEIDENTITY 52
#define LASTUPDATED 53
#define ORGANIZATION 54
#define CONTACTINFO 55
#define UINTEGER32 (56 | SYNTAX_MASK)
#define CURRENT     57
#define DEFINITIONS 58
#define END         59
#define SEMI        60
#define TRAPTYPE    61
#define ENTERPRISE  62
/*
 * #define DISPLAYSTR (63 | SYNTAX_MASK) 
 */
#define BEGIN       64
#define IMPORTS     65
#define EXPORTS     66
#define ACCNOTIFY   67
#define BAR         68
#define RANGE       69
#define CONVENTION  70
#define DISPLAYHINT 71
#define FROM        72
#define AGENTCAP    73
#define MACRO       74
#define IMPLIED     75
#define SUPPORTS    76
#define INCLUDES    77
#define VARIATION   78
#define REVISION    79
#define NOTIMPL	    80
#define OBJECTS	    81
#define NOTIFICATIONS	82
#define MODULE	    83
#define MINACCESS   84
#define PRODREL	    85
#define WRSYNTAX    86
#define CREATEREQ   87
#define NOTIFGROUP  88
#define MANDATORYGROUPS	89
#define GROUP	    90
#define OBJECT	    91
#define IDENTIFIER  92
#define CHOICE	    93
#define LEFTSQBRACK	95
#define RIGHTSQBRACK	96
#define IMPLICIT    97
#define APPSYNTAX	(98 | SYNTAX_MASK)
#define OBJSYNTAX	(99 | SYNTAX_MASK)
#define SIMPLESYNTAX	(100 | SYNTAX_MASK)
#define OBJNAME		(101 | SYNTAX_MASK)
#define NOTIFNAME	(102 | SYNTAX_MASK)
#define VARIABLES	103
#define UNSIGNED32	(104 | SYNTAX_MASK)
#define INTEGER32	(105 | SYNTAX_MASK)
struct tc {                     /* textual conventions */
    int             type;
    int             modid;
    char           *descriptor;
    char           *hint;
    struct enum_list *enums;
    struct range_list *ranges;
    char           *description;
};

struct objgroup {
    char           *name;
    int             line;
    struct objgroup *next;
};

struct tok {
    const char     *name;       /* token name */
    int             len;        /* length not counting nul */
    int             token;      /* value */
    int             hash;       /* hash of name */
    struct tok     *next;       /* pointer to next in hash table */
};

#define TOK_NUM 93
#define MODULE_MAP_NUM 22
#define HASHSIZE        32
#define NHASHSIZE    128
#define	NUMBER_OF_ROOT_NODES	3
#endif

    /*
     * non-aggregate types for tree end nodes 
     */
#define TYPE_OTHER          0
#define TYPE_OBJID          1
#define TYPE_OCTETSTR       2
#define TYPE_INTEGER        3
#define TYPE_NETADDR        4
#define TYPE_IPADDR         5
#define TYPE_COUNTER        6
#define TYPE_GAUGE          7
#define TYPE_TIMETICKS      8
#define TYPE_OPAQUE         9
#define TYPE_NULL           10
#define TYPE_COUNTER64      11
#define TYPE_BITSTRING      12
#define TYPE_NSAPADDRESS    13
#define TYPE_UINTEGER       14
#define TYPE_UNSIGNED32     15
#define TYPE_INTEGER32      16

#define TYPE_SIMPLE_LAST    16

#define TYPE_TRAPTYPE	    20
#define TYPE_NOTIFTYPE      21
#define TYPE_OBJGROUP	    22
#define TYPE_NOTIFGROUP	    23
#define TYPE_MODID	    24
#define TYPE_AGENTCAP       25
#define TYPE_MODCOMP        26

#define MIB_ACCESS_READONLY    18
#define MIB_ACCESS_READWRITE   19
#define	MIB_ACCESS_WRITEONLY   20
#define MIB_ACCESS_NOACCESS    21
#define MIB_ACCESS_NOTIFY      67
#define MIB_ACCESS_CREATE      48

#define MIB_STATUS_MANDATORY   23
#define MIB_STATUS_OPTIONAL    24
#define MIB_STATUS_OBSOLETE    25
#define MIB_STATUS_DEPRECATED  39
#define MIB_STATUS_CURRENT     57

#define	ANON	"anonymous#"
#define	ANON_LEN  strlen(ANON)

    struct tree    *read_module(const char *);
    struct tree    *read_mib(const char *);
    struct tree    *read_all_mibs(void);
    int             unload_module(const char *name);
    void            unload_all_mibs(void);
    void            init_mib_internals(void);
    int             add_mibfile(const char*, const char*, FILE *);
    int             add_mibdir(const char *);
    void            add_module_replacement(const char *, const char *,
                                           const char *, int);
    int             which_module(const char *);
    char           *module_name(int, char *);
    void            print_subtree(FILE *, struct tree *, int);
    void            print_ascii_dump_tree(FILE *, struct tree *, int);
    struct tree    *find_tree_node(const char *, int);
    const char     *get_tc_descriptor(int);
    const char     *get_tc_description(int);
    struct tree    *find_best_tree_node(const char *, struct tree *,
                                        u_int *);
    /*
     * backwards compatability 
     */
    struct tree    *find_node(const char *, struct tree *);
    struct tree    *find_node2(const char *, const char *); 
    struct module  *find_module(int);
    void            adopt_orphans(void);
    char           *snmp_mib_toggle_options(char *options);
    void            snmp_mib_toggle_options_usage(const char *lead,
                                                  FILE * outf);
    void            print_mib(FILE *);
    void            print_mib_tree(FILE *, struct tree *, int);
    int             get_mib_parse_error_count(void);
    int             snmp_get_token(FILE * fp, char *token, int maxtlen);
    struct tree    *find_best_tree_node(const char *name,
                                        struct tree *tree_top,
                                        u_int * match);

#ifdef __cplusplus
}
#endif
#endif                          /* PARSE_H */
