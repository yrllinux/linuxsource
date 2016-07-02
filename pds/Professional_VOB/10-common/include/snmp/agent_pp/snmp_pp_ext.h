/*_############################################################################
  _## 
  _##  snmp_pp_ext.h  
  _## 
  _##
  _##  AGENT++ API Version 3.5.23a
  _##  -----------------------------------------------
  _##  Copyright (C) 2000-2005 Frank Fock, Jochen Katz
  _##  
  _##  LICENSE AGREEMENT
  _##
  _##  WHEREAS,  Frank  Fock  and  Jochen  Katz  are  the  owners of valuable
  _##  intellectual  property rights relating to  the AGENT++ API and wish to
  _##  license AGENT++ subject to the  terms and conditions set forth  below;
  _##  and
  _##
  _##  WHEREAS, you ("Licensee") acknowledge  that Frank Fock and Jochen Katz
  _##  have the right  to grant licenses  to the intellectual property rights
  _##  relating to  AGENT++, and that you desire  to obtain a license  to use
  _##  AGENT++ subject to the terms and conditions set forth below;
  _##
  _##  Frank  Fock    and Jochen   Katz   grants  Licensee  a  non-exclusive,
  _##  non-transferable, royalty-free  license  to use   AGENT++ and  related
  _##  materials without  charge provided the Licensee  adheres to all of the
  _##  terms and conditions of this Agreement.
  _##
  _##  By downloading, using, or  copying  AGENT++  or any  portion  thereof,
  _##  Licensee  agrees to abide  by  the intellectual property  laws and all
  _##  other   applicable laws  of  Germany,  and  to all of   the  terms and
  _##  conditions  of this Agreement, and agrees  to take all necessary steps
  _##  to  ensure that the  terms and  conditions of  this Agreement are  not
  _##  violated  by any person  or entity under the  Licensee's control or in
  _##  the Licensee's service.
  _##
  _##  Licensee shall maintain  the  copyright and trademark  notices  on the
  _##  materials  within or otherwise  related   to AGENT++, and  not  alter,
  _##  erase, deface or overprint any such notice.
  _##
  _##  Except  as specifically   provided in  this  Agreement,   Licensee  is
  _##  expressly   prohibited  from  copying,   merging,  selling,   leasing,
  _##  assigning,  or  transferring  in  any manner,  AGENT++ or  any portion
  _##  thereof.
  _##
  _##  Licensee may copy materials   within or otherwise related   to AGENT++
  _##  that bear the author's copyright only  as required for backup purposes
  _##  or for use solely by the Licensee.
  _##
  _##  Licensee may  not distribute  in any  form  of electronic  or  printed
  _##  communication the  materials  within or  otherwise  related to AGENT++
  _##  that  bear the author's  copyright, including  but  not limited to the
  _##  source   code, documentation,  help  files, examples,  and benchmarks,
  _##  without prior written consent from the authors.  Send any requests for
  _##  limited distribution rights to fock@agentpp.com.
  _##
  _##  Licensee  hereby  grants  a  royalty-free  license  to  any  and   all 
  _##  derivatives  based  upon this software  code base,  that  may  be used
  _##  as a SNMP  agent development  environment or a  SNMP agent development 
  _##  tool.
  _##
  _##  Licensee may  modify  the sources  of AGENT++ for  the Licensee's  own
  _##  purposes.  Thus, Licensee  may  not  distribute  modified  sources  of
  _##  AGENT++ without prior written consent from the authors. 
  _##
  _##  The Licensee may distribute  binaries derived from or contained within
  _##  AGENT++ provided that:
  _##
  _##  1) The Binaries are  not integrated,  bundled,  combined, or otherwise
  _##     associated with a SNMP agent development environment or  SNMP agent
  _##     development tool; and
  _##
  _##  2) The Binaries are not a documented part of any distribution material. 
  _##
  _##
  _##  THIS  SOFTWARE  IS  PROVIDED ``AS  IS''  AND  ANY  EXPRESS OR  IMPLIED
  _##  WARRANTIES, INCLUDING, BUT NOT LIMITED  TO, THE IMPLIED WARRANTIES  OF
  _##  MERCHANTABILITY AND FITNESS FOR  A PARTICULAR PURPOSE  ARE DISCLAIMED.
  _##  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
  _##  INDIRECT,   INCIDENTAL,  SPECIAL, EXEMPLARY,  OR CONSEQUENTIAL DAMAGES
  _##  (INCLUDING,  BUT NOT LIMITED  TO,  PROCUREMENT OF SUBSTITUTE  GOODS OR
  _##  SERVICES; LOSS OF  USE,  DATA, OR PROFITS; OR  BUSINESS  INTERRUPTION)
  _##  HOWEVER CAUSED  AND ON ANY THEORY  OF  LIABILITY, WHETHER IN CONTRACT,
  _##  STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
  _##  IN  ANY WAY OUT OF  THE USE OF THIS  SOFTWARE,  EVEN IF ADVISED OF THE
  _##  POSSIBILITY OF SUCH DAMAGE. 
  _##
  _##
  _##  Stuttgart, Germany, Sat Jun 18 13:06:20 CEST 2005 
  _##  
  _##########################################################################*/

#ifndef _SNMP_PP_EXT_H_
#define _SNMP_PP_EXT_H_

#include <agent_pp/agent++.h>
#include <snmp_pp/oid.h>
#include <snmp_pp/pdu.h>
#include <snmp_pp/vb.h>
#include <snmp_pp/octet.h>
#include <snmp_pp/snmpmsg.h>


#define LENGTH_OF_IPADDRESS	4

#ifndef WIN32
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef SNMP_PP_NAMESPACE
#define NS_SNMP Snmp_pp::
#else
#define NS_SNMP
#endif

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/*--------------------------- class Oidx -----------------------------*/

/**
 * The Oidx class is a sub class of Oid that provides additional 
 * methods for manipulating OID values. AGENT++ uses Oidx instead
 * of Oid. 
 *
 * @version 3.4
 * @author Frank Fock
 */

class AGENTPP_DECL Oidx: public NS_SNMP Oid {
public:
	/**
	 * Construct an empty Object Identifier.
	 */
	Oidx();

	/**
	 * Construct an Object Identifier from a string.
	 *
	 * @param s - A oid string (e.g., ".x.y.z" or "x.y.z")
	 */
	Oidx(const char* s);

	/**
	 * Construct an Object Identifier from a long array.
	 *
	 * @param data - An array of long values.
	 * @param len - The length of the array.
	 */
	Oidx(const unsigned long *data, int len);

	/**
	 * Copy constructor.
	 *
	 * @param oid - Another object identifier.
	 */
	Oidx(const Oid& oid);

	/**
	 * Destructor
	 */
	virtual ~Oidx();

	/**
	 * Return a copy of the receiver oid without the n leftmost 
	 * subidentifiers.
	 * 
	 * @param n - The number of subidentifiers to cut of from left side.
	 * @return An Oidx object identifier.  
	 */
	Oidx		cut_left(const unsigned int) const;
	/**
	 * Return a copy of the receiver oid without the n rightmost 
	 * subidentifiers.
	 * 
	 * @param n - The number of subidentifiers to cut of from right side.
	 * @return An Oidx object identifier.  
	 */
	Oidx		cut_right(const unsigned int) const;


	/**
	 * Mask the receiver, i.e. zero those sub-identifiers for which
	 * a bit in the mask is set. 
	 * Each bit of the bit mask corresponds to a sub-identifier,
	 * with to most significant bit of the i-th octet of the octet
	 * string value corresponding to the (8*i - 7)-th sub-identifier,
	 * and the least significant bit of the i-th octet corresponding
	 * to the (8*i)-th sub-identifier.
	 *
	 * @param mask
	 *    an octet string.
	 * @return a reference to the receiver.
	 */
	Oidx&		mask(const NS_SNMP OctetStr&);


	/**
	 * Return a copy of the given oid without the n leftmost 
	 * subidentifiers.
	 * 
	 * @param o - An Oidx object identifier. 
	 * @param n - The number of subidentifiers to cut of from left side.
	 * @return An Oidx object identifier.  
	 */
	static Oidx	cut_left(const Oidx&, const unsigned int);
	/**
	 * Return a copy of the given oid without the n rightmost 
	 * subidentifiers.
	 * 
	 * @param o - An Oidx object identifier. 
	 * @param n - The number of subidentifiers to cut of from right side.
	 * @return An Oidx object identifier.  
	 */
	static Oidx	cut_right(const Oidx&, const unsigned int);

	/**
	 * Return the last subidentifier of the receiver.
	 * 
	 * @return A subidentifier.
	 */
	unsigned long	last() const;

	/**
	 * Clear the receiver.
	 */
	void		clear();

	unsigned long	operator[] (const unsigned int) const;
	unsigned long	&operator[] (const unsigned int);

	virtual Oidx&  	operator=(unsigned long l);

	Oidx		&operator+=(NS_SNMP IpAddress&); 
	Oidx		&operator+=(const char *); 
	Oidx		&operator+=(const unsigned long i);
	Oidx		&operator+=(const Oid &);

	/**
	 * Check if the receiver is in the subtree of a given oid.
	 *
	 * @param o - An Oidx object identifier.
	 * @return TRUE if the receiver is in the subtree of o, 
	 *         FALSE otherwise.
	 */
	int		in_subtree_of(const Oidx& o) const;

	/**
	 * Check if the receiver is root of a given oid.
	 *
	 * @param o - An Oidx object identifier.
	 * @return TRUE if the receiver is root of o, 
	 *         FALSE otherwise.
	 */
	int		is_root_of(const Oidx& o) const;

	/**
	 * Compare the receiver with another object identifier
	 * using a mask which defines to subidentifiers to be compared.
	 * The mask is used like defined in RFC2273 for the 
	 * snmpNotifyFilterMask.
	 *
	 * @param other
	 *    another object identifier to compare the receiver with.
	 * @param mask
	 *    an octet string containing a mask.
	 * @return 0 if both (masked) object identifiers are equal,
	 *    return 1 if the given object identifier is in the subtree
	 *    of the receiver, and return -1 otherwise. 
	 */
	int		compare(const Oidx&, const NS_SNMP OctetStr&) const; 

	/**
	 * Compare the receiver with another object identifier
	 * using a wildcard at the given subidentifier
	 *
	 * @param other
	 *    another object identifier to compare the receiver with.
	 * @param wildcard_subid
	 *    the subidentifier position (>=0) that is not compared
	 * @return 0 if both (masked) object identifiers are equal,
	 *    return 1 if the given object identifier is in the subtree
	 *    of the receiver, and return -1 otherwise. 
	 */
	int		compare(const Oidx&, u_int) const; 

	/**
	 * Return the receiver as an OctetStr. Every subidentifier is 
	 * interpreted as one char. Thus, all subidentifiers must be 
	 * between 0 and 255.
	 *
	 * @return An OctetStr.
	 */
	NS_SNMP OctetStr	as_string() const;

	/**
	 * Return an object identifier from a string. The first 
	 * subidentifier will contain the length of the string, each 
	 * following subidentifier represents one character.
	 *
	 * @param withExplicitLength
	 *    if FALSE there will be no preceeding subid containing
	 *    the length of the string will be generated.
	 * @return An Oidx.
	 */
	static Oidx	from_string(const NS_SNMP OctetStr&, boolean withLength=TRUE);

	/**
	 * Return the immediate lexicographic successor of the receiver.
	 *
	 * @return
	 *    an Oidx instance (receiver.0)
	 */
	Oidx		successor() const;

	/**
	 * Return the immediate lexicographic predecessor of the receiver.
	 *
	 * @return
	 *    an Oidx instance immediate preceding the receiver.
	 */
	Oidx		predecessor() const;

	/**
	 * Return the next peer of the receiver. Thus, add one to the
	 * last sub-identifier.
	 * Note: If the receiver's last sub-identifier is 0xFFFFFFFFul
	 * the returned object ID may not be valid.
	 *
	 * @return 
	 *    an Oidx instance.
	 */
	Oidx		next_peer() const;
};


/*--------------------------- class Vbx -----------------------------*/

/**
 * The Vbx class is a sub class of Vb that provides additional 
 * methods for manipulating SNMP variable bindings values. 
 * AGENT++ uses Vbx instead of Vb. Most of the base methods of Vb
 * are redefined as const methods. 
 *
 * @version 3.4
 * @author Frank Fock
 */


class AGENTPP_DECL Vbx: public NS_SNMP Vb {
public:
	Vbx(): Vb() { }
	Vbx(const NS_SNMP Vb& vb): Vb(vb) { }
	Vbx(const NS_SNMP Oid& oid): Vb(oid) { }

	/**
	 * Constructor with oid and value.
	 *
	 * @param oid 
	 *    the oid part of the new variable binding.
	 * @param value
	 *    the value part of the new variable binding.
	 */
	Vbx(const NS_SNMP Oid& oid, const NS_SNMP SnmpSyntax &val): Vb(oid) {
	  set_value(val); 
	}

	/**
	 * Return the oid value of the receiver.
	 *
	 * @return 
	 *    a copy of the oid value of the receiver.
	 */
	Oidx		get_oid() const;

	void		get_oid(Oidx&) const;

	NS_SNMP SmiUINT32	get_exception_status() const;

#ifndef SNMP_PP_V3
	SmiUINT32	get_syntax() const;
	void		set_syntax(SmiUINT32);

	int get_value( SnmpSyntax &val) const;
	int get_value( int &i) const;
	int get_value( long int &i) const;
	int get_value( unsigned long int &i) const;
	int get_value( unsigned char *ptr, unsigned long &len) const; 
	int get_value( unsigned char *ptr,	    // pointer to target space
		       unsigned long &len,          // returned len
		       unsigned long maxlen) const;       
	int get_value( char *ptr) const; 
#endif

	/**
	 * Clear the content of the variable binding. The content of
	 * the receiver will be the same as after creating it by Vbx()
	 */
	void		clear();

	/**
	 * Clone the value portion of the variable binding.
	 *
	 * @return
	 *    a pointer to a clone of the value of the receiver.
	 */
	NS_SNMP SnmpSyntax*	clone_value() const;
	Vbx*		clone() const { return new Vbx(*this); }

	friend int	operator==(const Vbx&, const Vbx&);

	/**
	 * Compare two Vbx arrays for equality.
	 *
	 * @param vbxArray1
	 *    an array of n Vbx variable bindings.
	 * @param vbxArray2
	 *    an array of n Vbx variable bindings.
	 * @param n 
	 *    the number of elements in the above arrays.
	 * @return 
	 *    TRUE if the oids and the values of both variable binding
	 *    arrays are equal.
	 */ 
	static boolean	equal(Vbx*, Vbx*, int);	

	/**
	 * Encode a given array of variable bindings into a octet stream
	 * using ASN.1 (BER). Note: The list of variable bindings is
	 * encoded as a sequence of oid/value pairs.
	 *
	 * @param vbs
	 *    a pointer to an array of variable bindings.
	 * @param size
	 *    the size of the above array.
	 * @param buf
	 *    returns a pointer to the character stream that holds the
	 *    BER encoding. Note that the user of this method is
	 *    responsible to free the memory allocated for buf.
	 * @param bufsize
	 *    the size of the returned character stream.
	 * @return
	 *    0 on success.
	 */
	static int	to_asn1(Vbx*, int, unsigned char*&, int&);

	/**
	 * Decode a character stream that holds BER encoded 
	 * sequence of variable bindings into an array of variable
	 * bindings.
	 *
	 * @param vbs
	 *    returns a pointer to an array of variable bindings.
	 * @param size
	 *    returns the size of the above array.
	 * @param buf
	 *    a pointer to a character stream holding a BER encoded 
	 *    sequence of variable bindings. After execution, this 
	 *    pointer will point to first character in buf after the
	 *    decoded sequence.
	 * @param bufsize
	 *    the number of characters left in the buffer (for decoding).
	 *    This will be decremented by the amount of decoded bytes.
	 */
	static int	from_asn1(Vbx*&, int&, unsigned char*&, int&);

 private:
	static unsigned char* asn_build_long_len_sequence(unsigned char *,
							  int*,
							  unsigned char,
							  int, int);
	static unsigned char* asn_build_long_length(unsigned char*,
						    int*,
						    int,
						    int);
};
 

/*------------------------- class OidxRange ---------------------------*/

/**
 * The OidxRange class provides functionality to define and compare
 * OID ranges.
 *
 * @version 3.03
 * @author Frank Fock
 */

class AGENTPP_DECL OidxRange {

public:
	/**
	 * Default constructor
	 */
	OidxRange();

	/**
	 * Create an OID range with lower and upper bound.
	 *
	 * @param lower
	 *    the lower bound of the oid range (must be lexicographically
	 *    less than upper)
	 * @param upper
	 *    the upper bound of the oid range
	 */
	OidxRange(const Oidx&, const Oidx&);

	/**
	 * Copy constructor
	 */
	OidxRange(const OidxRange&);

	/**
	 * Destructor
	 */
	virtual ~OidxRange();

	/**
	 * Clone
	 *
	 * @return
	 *    a pointer to a clone of the receiver.
	 */
	virtual OidxRange* clone() const;

	/**
	 * Compare the receiver with another OID range for equality
	 *
	 * @param other
	 *    another OidxRange instance
	 * @return
	 *    TRUE if lower and upper bounds of both OID ranges are equal
	 */
	virtual int	operator==(const OidxRange&) const;
	/**
	 * Compare the receiver with another OID range
	 *
	 * @param other
	 *    another OidxRange instance
	 * @return
	 *    TRUE if the upper bound of the receiver is less or equal to the
	 *    lower bound of other
	 */
	virtual int	operator<(const OidxRange&) const;
	/**
	 * Compare the receiver with another OID range
	 *
	 * @param other
	 *    another OidxRange instance
	 * @return
	 *    TRUE if the lower bound of the receiver is greater or equal to
	 *    the upper bound of other
	 */
	virtual int	operator>(const OidxRange&) const;

	/**
	 * Check whether an OID is within the receiver's range
	 *
	 * @param oid
	 *    an object identifier
	 * @return
	 *    TRUE if oid is greater or equal to the receiver's lower 
	 *    bound and less or equal to its upper bound.
	 */
	virtual int	includes(const Oidx&) const;

	/**
	 * Check whether an OID is within the receiver's range
	 *
	 * @param oid
	 *    an object identifier
	 * @return
	 *    TRUE if oid is greater or equal the receiver's lower 
	 *    bound and less than its upper bound.
	 */
	virtual int	includes_excl(const Oidx&) const;

	/**
	 * Check whether a range is covered by the receiver's range
	 *
	 * @param range
	 *    a OidxRange instance
	 * @return
	 *    TRUE if the lower bound of range is greater or equal the 
	 *    receiver's lower bound and if the upper bound of range is
	 *    less or equal the upper bound of the receiver, FALSE otherwise
	 */
	virtual boolean	covers(const OidxRange&) const;

	/**
	 * Check whether a range overlaps the receiver's range
	 *
	 * @param range
	 *    a OidxRange instance
	 * @return
	 *    TRUE if at least one oid is part of both ranges, FALSE otherwise
	 */
	virtual boolean	overlaps(const OidxRange&) const;

	/**
	 * Get lower bound
	 *
	 * @return
	 *    the lower bound object identifier
	 */
	virtual Oidx	get_lower() const;

	/**
	 * Get upper bound
	 *
	 * @return
	 *    the upper bound object identifier
	 */
	virtual Oidx	get_upper() const;

	Oidx lower, upper;
};


/*--------------------------- class Pdux -----------------------------*/

/**
 * The Pdux class is a sub class of Pdu that provides additional 
 * methods for manipulating SNMP PDUs. 
 * AGENT++ uses Pdux instead of Pdu, because Pdu does not allow to set
 * error status, error index and request id of a PDU. 
 *
 * @version 3.4
 * @author Frank Fock
 */


class AGENTPP_DECL Pdux: public NS_SNMP Pdu {
public:
	Pdux(): Pdu() { }
	Pdux(NS_SNMP Vb* pvbs, const int pvb_count): Pdu(pvbs, pvb_count) { }
	Pdux(const Pdu& pdu): Pdu(pdu) { }
	Pdux(const Pdux& pdu): Pdu(pdu) { }

	virtual ~Pdux() { }

	/**
	 * Set error status
	 *
	 * @param err
	 *    the new error status of the receiver
	 */ 
	void	set_error_status(int err)	    { error_status = err; }

	/**
	 * Set error index
	 *
	 * @param err
	 *    the new error index of the receiver
	 */ 
	void    set_error_index (int index)	    { error_index  = index;}
	/**
	 * Set request id
	 *
	 * @param err
	 *    the new request id of the receiver
	 */ 
	void    set_request_id(unsigned long reqid) { request_id   = reqid; }

	NS_SNMP Vb&	operator[](int i)	    { return *vbs[i]; }


	/** 
	 * Clear the Pdu contents (destruct and construct in one go)
	 */
	void    clear();

	/**
	 * Clear error status and error index of the receiver
	 */
	void    clear_error() { set_error_status(0); set_error_index(0); }

	// const redefinitions of originals: 
	Pdux&   operator+=(const NS_SNMP Vb&);

	int     get_vblist(Vbx*, const int) const; 

	/**
	 * Get request id 
	 *
	 * @return 
	 *    the request id of the receiver
	 */
	unsigned long get_request_id() const	    { return request_id; } 

	int	get_error_status() const { return error_status; }
	int	get_error_index() const  { return error_index; }

	/**
	 * Clone the receiver.
	 *
	 * @return
	 *    a pointer to a copy of the receiver.
	 */
	virtual Pdux*   clone() const { return new Pdux(*this); }
};


/*--------------------------- class Snmpx -----------------------------*/

/**
 * The Snmpx class is a sub class of Snmp that provides additional 
 * methods for sending and receiving SNMP messages. 
 * AGENT++ uses Snmpx instead of Snmp, because Snmp does not allow to
 * listen on an UDP port for incoming SNMP requests.  
 *
 * @version 3.4.4
 * @author Frank Fock
 */


class AGENTPP_DECL Snmpx: public NS_SNMP Snmp {
public:
	/**
	 * Construct a new SNMP session using the given UDP port.
	 *
	 * @param status
	 *    after creation of the session this parameter will 
	 *    hold the creation status.
	 * @param port
	 *    an UDP port to be used for the session
	 */
	Snmpx (int &status , u_short port): Snmp(status, port) {};

#ifdef SNMP_PP_WITH_UDPADDR
	/**
	 * Construct a new SNMP session using the given UDP address.
	 * Thus, binds the session on a specific IP address. 
	 *
	 * @param status
	 *    after creation of the session this parameter will 
	 *    hold the creation status.
	 * @param address
	 *    an UDP address to be used for the session
	 */	
	Snmpx(int& status, const NS_SNMP UdpAddress& addr): Snmp(status, addr) { }
#endif

#ifdef _SNMPv3		
	/**
	 * Receive a SNMP PDU
	 *
	 * @param timeout
	 *    wait for an incoming PDU until timeout is exceeded
	 * @param pdu
	 *    will contain the received SNMP PDU
	 * @param target
         *    the target (either CTarget or UTarget) will contain
	 *    - the UDP address of the sender of the received PDU
	 *    - the SNMP version of the received PDU
         *    - UTarget: security_model(community), security_name and 
         *               (authoritative) engine_id
	 * @return 
	 *   SNMP_CLASS_SUCCESS on success and SNMP_CLASS_ERROR,
	 *   SNMP_CLASS_TL_FAILED on failure.
	 */ 
	int receive(struct timeval*, Pdux&, NS_SNMP UTarget&);
#else
	/**
	 * Receive a SNMP PDU
	 *
	 * @param timeout
	 *    wait for an incoming PDU until timeout is exceeded
	 * @param pdu
	 *    will contain the received SNMP PDU
	 * @param form_address
	 *    the UDP address of the sender of the received PDU
	 * @param version
	 *    will contain the SNMP version of the received PDU
	 * @param community
	 *   will conatin the community (in case of SNMPv3 the 
	 *   security information) of the received PDU
	 * @return 
	 *   SNMP_CLASS_SUCCESS on success and SNMP_CLASS_ERROR,
	 *   SNMP_CLASS_TL_FAILED on failure.
	 */ 
	int receive(struct timeval*, Pdux&, NS_SNMP UdpAddress&, 
		    NS_SNMP snmp_version&, NS_SNMP OctetStr&);
#endif
#ifdef _SNMPv3
	/**
	 * Send o SNMP PDU
	 *
	 * @param pdu
	 *    the SNMP PDU to send
	 * @param target
	 *    the target (actually a CTarget or UTarget) that contains 
         *      - the receiver's UDP address
	 *      - the SNMP version to be used
         *      - CTarget: the community
         *      - UTarget: security_model, security_name and 
         *                 (if known) engine_id 
	 * @return 
	 *   SNMP_CLASS_SUCCESS on success and SNMP_CLASS_ERROR,
	 *   SNMP_CLASS_TL_FAILED on failure.
	 */
        int send (Pdux, NS_SNMP SnmpTarget*);
#else
	/**
	 * Send o SNMP PDU
	 *
	 * @param pdu
	 *    the SNMP PDU to send
	 * @param address
	 *    the receiver's UDP address
	 * @param version
	 *    the SNMP version to be used
	 * @param community
	 *    the community / security information to be used
	 * @return 
	 *   SNMP_CLASS_SUCCESS on success and SNMP_CLASS_ERROR,
	 *   SNMP_CLASS_TL_FAILED on failure.
	 */
        int send (Pdux, NS_SNMP UdpAddress, NS_SNMP snmp_version, NS_SNMP OctetStr);
#endif
	/**
	 * Get the port the request list is listening on.
	 *
	 * @return
	 *    a UDP port.
	 */
	u_short get_port();

	/**
	 * Return the socket descriptor of the socket used for
	 * incoming SNMP request. This is the socket the select()
	 * of the receive method listens on. This socket descriptor
	 * may be used to be included in other select controlled
	 * polling loops.
	 *
	 * @return
	 *    a socket descriptor.
	 */
#ifdef WIN32
	unsigned long	get_session_fds() { return iv_snmp_session; }
#else
        SNMPHANDLE	get_session_fds() { return iv_snmp_session; }
#endif


protected:
	unsigned long ProcessizedReqId(unsigned short);
	unsigned long MyMakeReqId();
	
	int snmp_engine( NS_SNMP Pdu &,		 // pdu to use
			 long int,   	 // # of non repititions
			 long int,   	 // # of max repititions
			 NS_SNMP SnmpTarget&,  	 // from this target
			 const NS_SNMP snmp_callback cb, 
			 const void * cbd);
};    

/*--------------------------- class Opaque -----------------------------*/

/**
 * The Opaque class represents the Opaque SNMP type. It is derived from
 * the SNMP++ class OctetStr and has the same interfaces and behavior,
 * except that its syntax is sNMP_SYNTAX_OPAQUE.
 *
 * @version 3.4.7
 * @author Frank Fock
 */


class AGENTPP_DECL Opaque: public NS_SNMP OctetStr 
{
 public:
  // constructor using no arguments
  Opaque();

  // constructor using a  string
  Opaque( const char * string);

  // constructor using an unsigned char *
  Opaque( const unsigned char * string, unsigned long int size);

  // constructor using another octet object
  Opaque( const OctetStr &octet);

  Opaque( const Opaque& opaque);

  virtual SnmpSyntax* clone() const { return new Opaque(*this); }

  virtual NS_SNMP SmiUINT32 get_syntax() const { return sNMP_SYNTAX_OPAQUE; };
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif







