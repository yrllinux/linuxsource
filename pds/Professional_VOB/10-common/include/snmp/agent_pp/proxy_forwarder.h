/*_############################################################################
  _## 
  _##  proxy_forwarder.h  
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


#ifndef _proxy_forwarder_h_
#define _proxy_forwarder_h_

#include <agent_pp/agent++.h>
#include <agent_pp/List.h>
#include <agent_pp/request.h>
#include <agent_pp/snmp_request.h> 

#ifdef _SNMPv3
#ifdef _PROXY_FORWARDER

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

class MibTableRow;

/**
 * The ProxyForwarder class represents a proxy forwarder instance 
 * as defined in RFC2573. 
 *
 * @author Frank Fock
 * @version 3.4.2
 */

class AGENTPP_DECL ProxyForwarder 
{
 public:
  typedef enum { ALL, WRITE, READ, NOTIFY, INFORM } pdu_type;
 
  /**
   * Construct a proxy forwarder for a given contextEngineID and
   * PDU type.
   *
   * @param contextEngineID
   *    a contextEngineID which must differ from the agents engineID.
   * @param pduType
   *    the PDU type(s) for which the proxy forwarder will be
   *    registered.
   */    
  ProxyForwarder(const NS_SNMP OctetStr&, pdu_type);

  ~ProxyForwarder();

  /**
   * Process a request and forward it to the target(s) determined
   * by exploring the SNMP-TARGET-MIB and SNMP-PROXY-MIB. 
   *
   * @param request
   *    a pointer to a Request instance.
   * @return 
   *    TRUE if the request could be proxied successfully (i.e.,
   *    an appropriate outgoing target could be found), or FALSE
   *    otherwise.
   */
  boolean		process_request(Request*);

  /**
   * Return a key value for the proxy forwarder application.
   *
   * @return
   *    a pointer to an Oidx instance whose first n-1 subidentifier
   *    represent the context engine ID and the nth subidentifier 
   *    denotes the PDU types registered for.
   */
  OidxPtr			key() { return &regKey; }



 protected:
  
  /**
   * Initialize SNMP session used to forward messages. This method
   * must be called before processRequest(..) is called the first time.
   */ 
  void		initialize();

  OidList<MibTableRow>*	get_matches(Request*);
  boolean		match_target_params(Request*, const NS_SNMP OctetStr&);
  boolean      		process_single(Pdux&, Request*);  
  boolean	       	process_multiple(Pdux&, Request*);  
  void			check_references();
  void			transform_pdu(const Pdux&, Pdux&);


  Oidx			regKey;
  SnmpRequestV3*       	snmp;
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
#endif

#endif
