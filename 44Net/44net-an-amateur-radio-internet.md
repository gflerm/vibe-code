# 44Net: Amateur Radio's Living Internet Laboratory

*An analysis of the 44Net/AMPRNet ecosystem, its history, technical models, and continuing relevance.*

44Net is a global community of licensed amateur radio operators who build and operate real networks using a shared pool of public Internet addresses. Also known as **AMPRNet** or simply **network 44**, it is neither a conventional Internet service provider nor a single centrally managed network. It is better understood as a long-running, distributed laboratory: a place where radio amateurs can learn by operating Internet-connected systems under real-world conditions.

The resource at its centre is unusual. 44Net uses globally routable IPv4 addresses that begin with `44`. Unlike the private addresses commonly used on home networks, these addresses can be reachable directly from the public Internet. That makes it possible to host services, operate remote stations, experiment with routing, interconnect radio systems, and build independent regional or international networks without treating network address translation (NAT) as an unavoidable boundary.

The result is an ecosystem rather than a product. A Raspberry Pi behind a residential connection, a club's linked repeater system, an emergency-data mesh, and an independently routed backbone can all participate in 44Net through different technical arrangements.

## From packet radio to 44Net

44Net traces its origin to 1981, when amateur radio operator Hank Magnuski, KA6M, requested Internet address space from Internet pioneer Jon Postel for amateur packet-radio networking. The allocation was the entire `44.0.0.0/8` block: 16,777,216 IPv4 addresses, created at a time when the Internet was small and such large allocations were far less contentious than they would be today.

The network became widely known as AMPRNet, short for Amateur Packet Radio Network. Its early identity reflected the technology of the period: low-speed packet links over amateur-radio frequencies, often operating at 1,200 bits per second. That heritage has not disappeared, but the network has expanded far beyond it. Modern 44Net projects may use microwave radio, fibre, cloud infrastructure, satellites, commercial Internet links, or combinations of these technologies.

Administration was formalised in 2011 with the creation of [Amateur Radio Digital Communications (ARDC)](https://www.ardc.net/), the nonprofit organisation that now carries the legal, fiduciary, and organisational responsibility for the address space and associated infrastructure.

In 2019, ARDC sold `44.192.0.0/10`, approximately 4.2 million addresses and one quarter of the original block. The proceeds funded an endowment used for ARDC's grants and operations. The retained space consists of:

- `44.0.0.0/9` — 8,388,608 addresses
- `44.128.0.0/10` — 4,194,304 addresses

Together, these prefixes contain **12,582,912 IPv4 addresses** reserved for amateur-radio use. The sale changed the size of the resource but not its central purpose. The [ARDC FAQ](https://www.ardc.net/about/faq/) explains both the transaction and the organisation's continuing stewardship of 44Net.

## Why public addresses matter

Most everyday devices reach the Internet through NAT: many private devices share one public address. NAT is practical, but it obscures the Internet's original end-to-end model and complicates inbound connections. A public 44Net address gives an operator a different learning environment.

With public addressing, participants can:

- expose a service directly to the Internet;
- experiment with routing and routing policy;
- create stable names and addresses for remote stations or repeaters;
- build routed subnets and multi-site networks;
- operate independently of a particular hosting provider; and
- observe the security and reliability demands of a real public network.

This last point is important. Public reachability is both a capability and a responsibility. A 44Net address does not create anonymity, privacy, or automatic security. Hosts must be patched, firewalls must be deliberate, services must be authenticated, and operators must monitor what they expose. The educational value of 44Net comes partly from confronting these genuine operational conditions.

## One address space, three operating models

The current wiki identifies three principal ways to provision 44Net. They share the same address space but differ in how traffic is carried and who controls the routing.

| Model | Best suited to | How it connects | Operator responsibility |
|---|---|---|---|
| **44Net Connect** | One device or a small network | WireGuard tunnel to a 44Net endpoint | Secure the host or subnet and maintain the tunnel |
| **IPIP Mesh** | Clubs, regions, and community networks | Cooperative mesh of IP-in-IP tunnels, usually with shared gateways | Run a mesh node and coordinate routes with peers |
| **BGP-announced subnet** | Autonomous or multi-site networks with routing expertise | Prefix advertised through the global Border Gateway Protocol | Operate the BGP edge, routing policy, filtering, and upstream relationships |

### 44Net Connect

[44Net Connect](https://wiki.ampr.org/wiki/44Net_Connect) is the simplest entry point. It creates a WireGuard tunnel from a device to a 44Net endpoint, allowing a laptop, router, Raspberry Pi, cloud server, or small lab to use a public 44Net address even when its ordinary Internet connection is behind NAT, carrier-grade NAT, a mobile provider, or a satellite service.

Despite using encrypted WireGuard transport, Connect is **not a commercial privacy VPN**. The tunnel carries 44Net traffic to a point of presence; it does not hide the participant or replace ordinary Internet access. Its purpose is public-network participation. This model lowers the barrier to entry because it does not require the participant to operate BGP or join a regional tunnel mesh.

### IPIP Mesh

The IPIP Mesh is the living descendant of the older AMPRNet tunnel network. Independently operated sites exchange traffic through IP-in-IP tunnels and coordinate routing as a community. Internet-bound or inbound traffic may pass through shared gateways, historically including infrastructure at the University of California San Diego.

This model suits persistent stations, clubs, and regional networks that value cooperative operation. It also reflects 44Net's decentralised nature: there is no single operator responsible for the whole mesh. Peering, gateway expectations, and routing practices depend on coordination among participants.

### BGP-announced networks

Operators with an autonomous system number, suitable routers, and an upstream or Internet-exchange relationship can integrate a 44Net allocation into their own routed infrastructure using BGP. This is the most independent model and the closest to operating at the Internet's routing layer.

It is appropriate for research networks, resilient backbones, multi-homed systems, and organisations already capable of managing routing policy and incidents. It is not simply a more advanced version of Connect; it serves a different operational setting. The 44Net wiki's BGP quick-start material was still marked as a stub when reviewed, so prospective operators should coordinate with the community and ARDC rather than treat archived instructions as a complete current procedure.

## What people build

44Net supports projects at very different scales. Individual operators host public services, monitoring systems, remote-control interfaces, and experiments in cloud “bring your own IP” routing. Clubs and volunteer groups link repeaters, share monitoring or DNS infrastructure, and build regional radio networks. Larger independent projects create microwave backbones, emergency-data systems, or globally routed multi-site networks.

Examples highlighted by the wiki include the Internet Radio Linking Project (IRLP), HamNET's broadband RF networks, and the AREDN emergency-data mesh. These examples should not be mistaken for components of one centrally designed platform. They are separate projects that use or intersect with 44Net in different ways. That diversity is one of the network's defining features.

The network therefore makes most sense as a commons with technical boundaries. ARDC maintains the address resource and core services; participants decide what to build, maintain their systems, and collaborate where their projects meet.

## Addresses, names, and the Portal

The [44Net Portal](https://portal.ampr.org/) is the administrative gateway for account management, callsign verification, address requests, and related services. A typical new participant creates an account, verifies a current amateur-radio callsign, chooses an operating model, and then requests or provisions the resources appropriate to the project.

44Net also operates DNS under `ampr.org`. A verified participant can generally claim a callsign-based subdomain such as `callsign.ampr.org`, then add records for hosts, gateways, repeaters, or services. Operators who need more control may delegate their subdomain to their own authoritative name servers. DNS turns stable public addresses into discoverable services and is therefore a practical part of the ecosystem, not merely an administrative extra.

Participation is generally available without charge to licensed radio amateurs for amateur-radio experimentation and non-commercial, community-benefit purposes. An address assignment is not permanent private property. Good stewardship means requesting only what a project can justify, maintaining accurate information, operating responsibly, and returning unused space when a project ends. Applicants should consult the current Portal terms and policy pages because eligibility and operational requirements can evolve.

## What 44Net is — and is not

Several distinctions clarify the network's purpose:

- **It is public infrastructure, not free consumer Internet access.** A participant still needs an underlying radio or Internet connection.
- **It is not a managed hosting service.** Participants operate and secure their own systems.
- **It is not one physical network.** Radio links, Internet tunnels, and directly routed prefixes coexist.
- **It is not defined by one protocol.** Packet radio is historically central, but WireGuard, IP-in-IP, BGP, microwave, fibre, and other technologies all have roles.
- **It is not a privacy service.** A public address is intended to make a system reachable and attributable, not anonymous.

These boundaries explain why 44Net remains valuable. It gives amateurs access to scarce public address space while preserving the experimental, self-directed character of amateur radio.

## Continuing relevance

IPv4 scarcity makes 44Net historically exceptional, but its relevance is not limited to owning a large legacy address block. Its deeper value is institutional and educational. It lets volunteers practise addressing, naming, security, peering, routing policy, remote operations, and incident response in an environment that crosses the boundary between radio and the public Internet.

It also preserves an early Internet idea: networks can be built by communities of capable operators rather than consumed only as services from large providers. Modern access methods such as 44Net Connect broaden participation, while the IPIP Mesh retains collaborative continuity and BGP permits genuine routing autonomy.

The wiki itself reflects a network in transition. Its main conceptual pages are current and coherent, but some procedural documentation was still incomplete at the time of review. The IPIP Mesh and BGP quick-start pages were explicitly labelled as stubs, while the eligibility, governance, and policy pages provided orientation and planned coverage rather than comprehensive guidance. Older documentation remains available in an archive and may still contain useful technical context, but it should not automatically be treated as current policy. The safest starting point is the [44Net Main Page](https://wiki.ampr.org/wiki/Main_Page), followed by the current [getting-started guide](https://wiki.ampr.org/wiki/GetStarted), the Portal, and community discussion channels.

## Conclusion

44Net is a rare combination of Internet history, public infrastructure, and volunteer experimentation. Born from packet radio in 1981, it has evolved into a globally distributed environment supporting everything from a single tunneled host to an independently routed backbone. ARDC safeguards the common address resource, but the network's character comes from the licensed amateurs who build, connect, document, secure, and eventually retire their projects.

Its most important lesson is that networking is not only something delivered by an ISP or hidden behind a cloud dashboard. It is something people can learn deeply by operating together. In that sense, 44Net remains faithful to both amateur radio and the early Internet: open-ended, practical, decentralised, and sustained by shared technical stewardship.

## Sources and further reading

- [44Net Wiki — Main Page](https://wiki.ampr.org/wiki/Main_Page)
- [44Net Wiki — About 44Net](https://wiki.ampr.org/wiki/About_44Net)
- [44Net Wiki — Provisioning Methods](https://wiki.ampr.org/wiki/Provisioning_Methods)
- [44Net Wiki — 44Net Connect](https://wiki.ampr.org/wiki/44Net_Connect)
- [44Net Wiki — Get Started](https://wiki.ampr.org/wiki/GetStarted)
- [44Net Wiki — DNS](https://wiki.ampr.org/wiki/DNS)
- [ARDC — Frequently Asked Questions](https://www.ardc.net/about/faq/)
- [ARDC — 44Net Survey Assessment](https://www.ardc.net/wp-content/uploads/ARDC-44net-Survey-Assessment-Results-sm.pdf)

*Sources reviewed 22 July 2026. Because the wiki is actively being revised, readers should verify procedural and policy details against the live pages before deploying a system or requesting resources.*
