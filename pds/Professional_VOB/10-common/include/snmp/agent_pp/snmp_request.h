/*_############################################################################
  _## 
  _##  snmp_request.h  
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


#ifndef snmp_request_h_
#define snmp_request_h_

#include <snmp_pp/snmperrs.h>
#include <snmp_pp/address.h>
#include <snmp_pp/target.h>
#include <agent_pp/snmp_pp_ext.h>
#include <snmp_pp/snmp_pp.h>

#define MIN_REQUEST_ID	10000
#define DEFAULT_RETRIES 1
#define DEFAULT_TIMEOUT 900

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


class AGENTPP_DECL InformInfo {

 public:
	InformInfo(NS_SNMP CTarget&, Vbx*, int, const Oidx&);
	~InformInfo();
	
	NS_SNMP CTarget	   target;
	Vbx*       vbs;
	int        sz;
	Oidx       oid;
};


class AGENTPP_DECL SnmpRequest {
    friend class SnmpRequestV3;
public: 
       static int process(int, const NS_SNMP UdpAddress&, Vbx*, int& sz, Vbx*, int&,
			  const NS_SNMP OctetStr&, const int=0, const int=0);
       static int process_trap(NS_SNMP SnmpTarget&, Vbx*, int sz, 
			       const Oidx&, const Oidx&, boolean = FALSE);

       static int get (const NS_SNMP UdpAddress&, Vbx*, int sz, int&);
       static int next(const NS_SNMP UdpAddress&, Vbx*, int sz, int&);
       static int getbulk(const NS_SNMP UdpAddress&, Vbx*, int& sz, Vbx*, int&, 
			  const int, const int);
       static int set (const NS_SNMP UdpAddress&, Vbx*, int sz, int&);

       static int get (const NS_SNMP UdpAddress&, Vbx*, int sz, int&, const NS_SNMP OctetStr&);
       static int next(const NS_SNMP UdpAddress&, Vbx*, int sz, int&, const NS_SNMP OctetStr&);
       static int getbulk(const NS_SNMP UdpAddress&, Vbx*, int& sz, Vbx*, int&, 
			  const NS_SNMP OctetStr&, const int, const int);
       static int gettable(const NS_SNMP UdpAddress&, Vbx*, int sz, Vbx*, int&, 
			   const NS_SNMP OctetStr&, const int);
       static int set (const NS_SNMP UdpAddress&, Vbx*, int sz, int&, const NS_SNMP OctetStr&);

       static int trap(NS_SNMP SnmpTarget&, Vbx*, int sz, const Oidx&, const Oidx&);
       static void inform(NS_SNMP CTarget&, Vbx*, int sz, const Oidx&);

 protected:
       static Snmpx *get_new_snmp(int &status);
};

#ifdef _SNMPv3

/**
 * The SnmpRequestV3 class can be used to send SNMPv1/v2c/v3 messages
 * synchronously to other SNMP entities.
 *
 * @author Frank Fock
 * @version 3.4.2
 */

class AGENTPP_DECL SnmpRequestV3 {

 public:
	/**
	 * Construct a SnmpRequestV3 session. If there could not be
	 * opened any SNMP port, a error message will be printed to
	 * stdout and the program will exit. 
	 * This behavior can be avoided, when the static method
	 * send(..) is used instead.
	 */
	SnmpRequestV3();
	/**
	 * Destructor
	 */
	~SnmpRequestV3();

	/**
	 * Static method to send a SNMP request synchronously.
	 *
	 * @param target
	 *    a UTarget instance denoting the target address for the request.
	 * @param pdu
	 *    the Pdux instance to send.
	 * @return
	 *    SNMP_ERROR_SUCCESS on success or any other SNMP error code on
	 *    failure. Additonally, if the SNMP port could not be initialized
	 *    a SNMP class error will be returned.
	 */
	static int send(NS_SNMP UTarget&, Pdux&);

	/**
	 * Method to send a SNMP request synchronously.
	 *
	 * @param target
	 *    a UTarget instance denoting the target address for the request.
	 * @param pdu
	 *    the Pdux instance to send.
	 * @return
	 *    SNMP_ERROR_SUCCESS on success or any other SNMP error code on
	 */
        int send_request(NS_SNMP UTarget&, Pdux&);

 protected:

	Snmpx* snmp;
};
#endif
#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
