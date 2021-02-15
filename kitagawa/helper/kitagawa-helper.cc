/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 Yuki Kitagawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Yuki Kitagawa <is0303vk@ed.ritsumei.ac.jp>
 *         Ritsumeikan University, Shiga, Japan
 */
#include "kitagawa-helper.h"
#include "ns3/kitagawa-routing-protocol.h"
#include "ns3/node-list.h"
#include "ns3/names.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-list-routing.h"

namespace ns3
{

KitagawaHelper::KitagawaHelper() : 
  Ipv4RoutingHelper ()
{
  m_agentFactory.SetTypeId ("ns3::kitagawa::KitagawaRoutingProtocol");
}

KitagawaHelper* 
KitagawaHelper::Copy (void) const 
{
  return new KitagawaHelper (*this); 
}

Ptr<Ipv4RoutingProtocol> 
KitagawaHelper::Create (Ptr<Node> node) const
{
  Ptr<kitagawa::KitagawaRoutingProtocol> agent = m_agentFactory.Create<kitagawa::KitagawaRoutingProtocol> ();
  node->AggregateObject (agent);
  return agent;
}

void 
KitagawaHelper::Set (std::string name, const AttributeValue &value)
{
  m_agentFactory.Set (name, value);
}

int64_t
KitagawaHelper::AssignStreams (NodeContainer c, int64_t stream)
{
  int64_t currentStream = stream;
  Ptr<Node> node;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      node = (*i);
      Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
      NS_ASSERT_MSG (ipv4, "Ipv4 not installed on node");
      Ptr<Ipv4RoutingProtocol> proto = ipv4->GetRoutingProtocol ();
      NS_ASSERT_MSG (proto, "Ipv4 routing not installed on node");
      Ptr<kitagawa::KitagawaRoutingProtocol> kitagawa = DynamicCast<kitagawa::KitagawaRoutingProtocol> (proto);
      if (kitagawa)
        {
          currentStream += kitagawa->AssignStreams (currentStream);
          continue;
        }
      // Kitagawa may also be in a list
      Ptr<Ipv4ListRouting> list = DynamicCast<Ipv4ListRouting> (proto);
      if (list)
        {
          int16_t priority;
          Ptr<Ipv4RoutingProtocol> listProto;
          Ptr<kitagawa::KitagawaRoutingProtocol> listKitagawa;
          for (uint32_t i = 0; i < list->GetNRoutingProtocols (); i++)
            {
              listProto = list->GetRoutingProtocol (i, priority);
              listKitagawa = DynamicCast<kitagawa::KitagawaRoutingProtocol> (listProto);
              if (listKitagawa)
                {
                  currentStream += listKitagawa->AssignStreams (currentStream);
                  break;
                }
            }
        }
    }
  return (currentStream - stream);
}

}
