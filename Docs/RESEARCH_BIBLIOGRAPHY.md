# Research bibliography and evidence mapping

The consolidated source list for the DragonCycle design, mapped to the
constraints in `BIOMECHANICS.md`. Compiled 18 July 2026.

The mapping matters more than the list. A constraint with a source behind it
can be argued with on the evidence; a constraint without one is a design
opinion wearing a lab coat, and should be labelled as such so that nobody —
human or agent — treats it as settled.

## Taxonomy note

*Dragon*, *wyvern*, *wyrm*, *lindworm*, *amphiptere*, and *drake* do not form
one universal historical taxonomy. Their forms vary by period, region,
heraldic tradition, and modern fantasy. In this project the names are design
families inspired by historical sources, not claims about a single
authoritative classification.

## Evidence status by constraint

Gaps closed July 2026. Every constraint now carries either a source or an
explicit *reasoned* label. The distinction is kept sharp on purpose: a rule
labelled *sourced* can be argued with on evidence, and a rule labelled
*reasoned* has to be argued with on design grounds. Mislabelling one as the
other is how a tuning opinion acquires false authority.

| # | Constraint | Status | Sources |
|---|---|---|---|
| 1 | Wing loading trades speed against agility | Sourced | 19, 20, 22, 23 |
| 2 | Launch is gated and anatomy-specific | Sourced | 17, 18 |
| 3 | Bite force scales with size | Sourced | 24 |
| 4 | Secured bite converts to rotational tearing | Sourced | 25, 29 |
| 5 | Neck buys reach and angle, costs inertia | **Sourced** | 86–92 |
| 6 | Interception is set up, not steered | Sourced (guidance law); the no-re-aim rule is reasoned | 47–50, 93, 94 |
| 7 | Sensing tracks foraging mode | **Sourced within squamates**; cross-clade is reasoned | 51–59, 95–97 |
| 8 | Age is a curve with three axes | Direction sourced; **reptile performance senescence is an open literature gap** | 30, 60–67, 98–102 |
| — | Assessment, resource value, accumulated cost | Sourced | 68–85 |
| — | Skill as an axis separable from raw ability | **Sourced** | 81, 82, 103, 104 |

### What is still not evidence

Three items. The first is inference; the other two are absences in the
literature, which is a different and more interesting problem.

**The no-re-aim rule inside §6 — reasoned.** The mechanism is well
characterised: intercepting a manoeuvring target needs a low navigation
constant and a load factor two to three times the prey's, and response delay
plus vision and control error bound when interception remains achievable (47).
But no study measures an abort point inside a stoop, a minimum redirect time,
or a re-attack interval after a miss. Attack-rate data exist — peregrines took
205 dunlin in 1369 attacks, a success rate rising from about 11% in October to
15% in midwinter (93) — which tells us misses are the norm, not what a miss
costs. Closing this needs onboard-camera work with timestamped abort events.
The platform exists; the measurement has not been published.

**Reptile performance senescence — a genuine literature gap, resolved by
reference class (ADR-014) rather than by waiting.** No study
measures sprint speed, bite force, endurance, or recovery against advancing
adult age in any squamate, crocodilian, or chelonian. The nearest miss (98)
compares one-year against two-to-three-year adults and finds essentially no
decline, over an age range too narrow to test senescence at all. The reptile
performance literature is overwhelmingly ontogenetic — hatchling to adult,
where performance rises (99) — and Reinke (30) measures demographic
senescence, not physical. A review from the field concedes the point (102).
Use the cricket (100) and guppy (101) proxies alongside the large-bird and
mammal curve (62, 63). ADR-014 sets the reference class: for a six-tonne
powered flier with high metabolic cost, large endotherms are the better
analogue than a turtle.

**The cross-clade sensory comparison — reasoned.** Within squamates the
principle is settled: across 94 species, foraging mode rather than phylogeny
drives chemosensory investment (97). Extending that to a raptor-versus-serpent
contrast is extrapolation, because no single analysis scores sensory
investment across those body plans.

Do not let an ontogenetic result stand in for a senescent one. Source 99 shows
bite force *rising* with age and is flagged in the list for that reason — it
measures the opposite of what §8 claims.

## Sources not yet used by the design

Three sources support mechanics the kit currently does not implement. Two are
now scheduled.

**Constriction (26, 27, 28)** — constriction induces rapid circulatory arrest;
pressure scales with body diameter; and snakes *modulate* constriction from
the prey's heartbeat rather than squeezing for a fixed interval. That last one
argues the arch-serpent's coil drag should be a feedback-driven contest, not
an authored duration. Scheduled as S2.3.

**Elongated-body hemodynamics (31, 32)** — heart-to-head distance and caudal
blood pooling constrain long-bodied animals holding a head-up posture. This
gives the nine-segment neck an *active* cost rather than only passive inertia:
a sustained raised-neck guard or threat display should drain and have a limit.
Scheduled as S2.6, and a better drawback than the one currently written.

**Jaw slenderness (29)** — long slender jaws trade structural resistance
against other feeding performance. This retroactively supports a call already
made on intuition: the wyvern and arch-serpent cannot perform the drake's
secured-bite rotation, because a slender jaw cannot take the torsion. Noted in
`BIOMECHANICS.md` §4.

## A. Dragon mythology, iconography, and terminology

1. British Library — The Anatomy of a Dragon. https://www.bl.uk/stories/blogs/posts/the-anatomy-of-a-dragon
2. British Library — What is a bestiary? https://www.bl.uk/stories/blogs/posts/what-is-a-bestiary
3. British Library — Hwæt! Beowulf online. https://www.bl.uk/stories/blogs/posts/beowulf-online
4. British Library — Cotton MS Vitellius A XV catalogue record. https://searcharchives.bl.uk/catalog/040-001102971
5. The Met — Seven Necessary Dragons. https://www.metmuseum.org/perspectives/seven-necessary-dragons
6. The Met — East Asian Cultural Exchange in Tiger and Dragon Paintings. https://www.metmuseum.org/essays/east-asian-cultural-exchange-in-tiger-and-dragon-paintings
7. Smarthistory — Eleven Dragons handscroll. https://smarthistory.org/eleven-dragons-handscroll/
8. The Met — Vase with Dragon amid Clouds. https://www.metmuseum.org/art/collection/search/42364
9. Theoi — Lernaean Hydra. https://www.theoi.com/Ther/DrakonHydra.html
10. Theoi — Bestiary. https://www.theoi.com/greek-mythology/bestiary.html
11. Merriam-Webster — Wyvern. https://www.merriam-webster.com/dictionary/wyvern
12. Merriam-Webster — Dragon. https://www.merriam-webster.com/dictionary/dragon
13. Merriam-Webster — Lindworm. https://www.merriam-webster.com/dictionary/lindworm
14. Merriam-Webster — Worm. https://www.merriam-webster.com/dictionary/worm
15. Wiktionary Citations — Amphiptere (terminology reference only). https://en.wiktionary.org/wiki/Citations:amphiptere
16. 1911 Encyclopaedia Britannica — Dragon. https://en.wikisource.org/wiki/1911_Encyclop%C3%A6dia_Britannica/Dragon

## B. Flight, size, locomotion, and manoeuvrability

17. Witton, M. P. & Habib, M. B. (2010). On the Size and Flight Diversity of Giant Pterosaurs. *PLOS ONE* 5(11): e13982. https://doi.org/10.1371/journal.pone.0013982
18. Habib, M. B. (2008). Comparative Evidence for Quadrupedal Launch in Pterosaurs. *Zitteliana* B 28: 159–166. https://epub.ub.uni-muenchen.de/12011/
19. Harvey, C. et al. (2022). Birds can transition between stable and unstable states via wing morphing. *Nature*. https://pmc.ncbi.nlm.nih.gov/articles/PMC8942853/
20. Chin, D. D. & Lentink, D. (2016). Evolution of avian flight: muscles and constraints on performance. *Phil. Trans. R. Soc. B*. https://pmc.ncbi.nlm.nih.gov/articles/PMC4992707/
21. Cheney, J. A. et al. (2017). Inspiration for wing design. *J. R. Soc. Interface*. https://pmc.ncbi.nlm.nih.gov/articles/PMC5493806/
22. Shyy, W. et al. (2008). Beyond robins: aerodynamic analyses of animal flight. *J. R. Soc. Interface*. https://pmc.ncbi.nlm.nih.gov/articles/PMC2706015/
23. Galiński, C. & Żbikowski, R. (2015). Forward flight of birds revisited, Part 1. *R. Soc. Open Sci.* https://pmc.ncbi.nlm.nih.gov/articles/PMC4448904/

## C. Biting, grappling, constriction, and predator combat

24. Erickson, G. M. et al. (2012). Insights into the Ecology and Evolutionary Success of Crocodilians Revealed through Bite-Force and Tooth-Pressure Experimentation. *PLOS ONE* 7(3): e31781. https://doi.org/10.1371/journal.pone.0031781
25. Fish, F. E. et al. (2007). Death roll of the alligator: mechanics of twist feeding in water. *J. Exp. Biol.* 210: 2811–2818. https://doi.org/10.1242/jeb.004267
26. Boback, S. M. et al. (2015). Snake constriction rapidly induces circulatory arrest in rats. *J. Exp. Biol.* 218: 2279–2288. https://doi.org/10.1242/jeb.121384
27. Penning, D. A. & Dartez, S. F. (2015). The big squeeze: scaling of constriction pressure in two of the world's largest snakes. *J. Exp. Biol.* https://doi.org/10.1242/jeb.127449
28. Boback, S. M. et al. (2012). Snake modulates constriction in response to prey's heartbeat. *Biol. Lett.* 8(3): 473–476. https://doi.org/10.1098/rsbl.2011.1105
29. Walmsley, C. W. et al. (2013). Why the Long Face? The Mechanics of Mandibular Symphysis Proportions in Crocodiles. *PLOS ONE* 8(1): e53873. https://doi.org/10.1371/journal.pone.0053873

## D. Aging and elongated-body physiology

Note: sources 31 and 32 concern elongated-body hemodynamics, not aging. Only
source 30 bears on aging.

30. Reinke, B. A. et al. (2022). Diverse aging rates in ectothermic tetrapods. *Science* 376(6600): 1459–1466. https://doi.org/10.1126/science.abm0151
31. Lillywhite, H. B. & Gallagher, K. P. (1985). Hemodynamic adjustments to head-up posture in the partly arboreal snake, *Elaphe obsoleta*. *J. Exp. Zool.* https://doi.org/10.1002/jez.1402350303
32. Lillywhite, H. B. et al. (2004). Independent effects of heart-head distance and caudal blood pooling on blood pressure regulation in aquatic and terrestrial snakes. *J. Exp. Biol.* https://pubmed.ncbi.nlm.nih.gov/15010481/

## E. Unreal Engine and game AI

33. Epic — Unreal MCP in Unreal Editor (UE 5.8). https://dev.epicgames.com/documentation/unreal-engine/unreal-mcp-in-unreal-editor
34. Epic — Unreal Engine 5.8 Release Notes. https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-5-8-release-notes
35. Epic — Gameplay Systems. https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-systems-in-unreal-engine
36. Epic — Gameplay Framework. https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-framework-in-unreal-engine
37. Epic — Gameplay Ability System. https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine
38. Epic — Using Gameplay Abilities. https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-abilities-in-unreal-engine
39. Epic — StateTree Quick Start Guide. https://dev.epicgames.com/documentation/en-us/unreal-engine/statetree-quick-start-guide
40. Epic — Behavior Trees. Listed as an alternative architecture; deliberately not used, see ADR-007. https://dev.epicgames.com/documentation/en-us/unreal-engine/behavior-trees-in-unreal-engine
41. Epic — Scripting and Automating the Unreal Editor. https://dev.epicgames.com/documentation/en-us/unreal-engine/scripting-and-automating-the-unreal-editor

## F. Claude Code, MCP, and Higgsfield

42. Claude Code documentation. https://code.claude.com/docs/en/overview
43. Claude Code CLI reference. https://code.claude.com/docs/en/cli-reference
44. Claude Code — connecting to tools via MCP. https://code.claude.com/docs/en/mcp
45. Anthropic — Claude Code Best Practices. https://www.anthropic.com/engineering/claude-code-best-practices
46. Higgsfield — CLI and agent integration. https://higgsfield.ai/cli

Entries 42–44 were listed under `docs.anthropic.com/en/docs/claude-code/…` in
the source bibliography. Those paths are stale; the docs live at
`code.claude.com/docs` and the CLI page is `cli-reference`, not `cli-usage`.

## H. High-speed aerial interception

47. Mills, R., Hildenbrandt, H., Taylor, G. K. & Hemelrijk, C. K. (2018). Physics-based simulations of aerial attacks by peregrine falcons reveal that stooping at high speed maximizes catch success against agile prey. *PLOS Comput. Biol.* 14(4): e1006044. https://doi.org/10.1371/journal.pcbi.1006044
48. Brighton, C. H., Thomas, A. L. R. & Taylor, G. K. (2017). Terminal attack trajectories of peregrine falcons are described by the proportional navigation guidance law of missiles. *PNAS* 114(51): 13495–13500. https://doi.org/10.1073/pnas.1714532114
49. Brighton, C. H. & Taylor, G. K. (2019). Hawks steer attacks using a guidance system tuned for close pursuit of erratically manoeuvring targets. *Nature Communications* 10: 2462. https://doi.org/10.1038/s41467-019-10454-z
50. Kempton, J. A., Brighton, C. H., France, J., KleinHeerenbrink, M., Miñano, S., Shelton, J. & Taylor, G. K. (2023). Visual versus visual-inertial guidance in hawks pursuing terrestrial targets. *J. R. Soc. Interface* 20(203): 20230071. https://doi.org/10.1098/rsif.2023.0071

## I. Sensory ecology

51. Potier, S., Mitkus, M. & Kelber, A. (2020). Visual adaptations of diurnal and nocturnal raptors. *Semin. Cell Dev. Biol.* 106: 116–126. https://doi.org/10.1016/j.semcdb.2020.05.004
52. Mitkus, M., Potier, S., Martin, G. R., Duriez, O. & Kelber, A. (2018). Raptor Vision. *Oxford Research Encyclopedia of Neuroscience.* https://doi.org/10.1093/acrefore/9780190264086.013.232
53. O'Rourke, C. T., Hall, M. I., Pitlik, T. & Fernández-Juricic, E. (2010). Hawk eyes I: diurnal raptors differ in visual fields and degree of eye movement. *PLOS ONE* 5(9): e12802. https://doi.org/10.1371/journal.pone.0012802
54. Gracheva, E. O. et al. (2010). Molecular basis of infrared detection by snakes. *Nature* 464: 1006–1011. https://doi.org/10.1038/nature08943
55. Leitch, D. B. & Catania, K. C. (2012). Structure, innervation and response properties of integumentary sensory organs in crocodilians. *J. Exp. Biol.* 215(23): 4217–4230. https://doi.org/10.1242/jeb.076836 — verified. PMC4074209 is correct despite looking late for a 2012 paper; PMC accessions track deposit date, not publication date.
56. Soares, D. (2002). An ancient sensory organ in crocodilians. *Nature* 417: 241–242. https://doi.org/10.1038/417241a
57. Christensen, C. B., Christensen-Dalsgaard, J., Brandt, C. & Madsen, P. T. (2012). Hearing with an atympanic ear: good vibration and poor sound-pressure detection in the royal python, *Python regius*. *J. Exp. Biol.* 215(2): 331–342. https://doi.org/10.1242/jeb.062539
58. Schwenk, K. (1995). Of tongues and noses: chemoreception in lizards and snakes. *Trends Ecol. Evol.* 10(1): 7–12. https://doi.org/10.1016/S0169-5347(00)88953-3
59. Kubie, J. L. & Halpern, M. (1979). Chemical senses involved in garter snake prey trailing. *J. Comp. Physiol. Psychol.* 93(4): 648–667. https://doi.org/10.1037/h0077606

## J. Senescence and performance decline

60. Nussey, D. H., Froy, H., Lemaître, J.-F., Gaillard, J.-M. & Austad, S. N. (2013). Senescence in natural populations of animals: widespread evidence and its implications for bio-gerontology. *Ageing Res. Rev.* 12(1): 214–225. https://doi.org/10.1016/j.arr.2012.07.004
61. Nussey, D. H., Coulson, T., Festa-Bianchet, M. & Gaillard, J.-M. (2008). Measuring senescence in wild animal populations: towards a longitudinal approach. *Funct. Ecol.* 22(3): 393–406. https://doi.org/10.1111/j.1365-2435.2008.01408.x
62. Ganse, B., Ganse, U., Degens, H. et al. (2018). Age-related changes in locomotor performance reveal a similar pattern for *Caenorhabditis elegans*, *Mus domesticus*, *Canis familiaris*, *Equus caballus*, and *Homo sapiens*. *J. Gerontol. A* 73(5): 455–460. https://doi.org/10.1093/gerona/glw136
63. Larsson, L., Degens, H., Li, M. et al. (2019). Sarcopenia: Aging-Related Loss of Muscle Mass and Function. *Physiol. Rev.* 99(1): 427–511. https://doi.org/10.1152/physrev.00061.2017
64. Husak, J. F. & Fox, S. F. (2006). Field use of maximal sprint speed by collared lizards (*Crotaphytus collaris*): compensation and sexual selection. *Evolution* 60(9): 1888–1895. https://doi.org/10.1554/05-648.1
65. Husak, J. F. (2006). Does survival depend on how fast you can run or how fast you do run? *Funct. Ecol.* 20(6): 1080–1086. https://doi.org/10.1111/j.1365-2435.2006.01195.x
66. Bronikowski, A. M. (2008). The evolution of aging phenotypes in snakes: a review and synthesis with new data. *Age* 30: 169–176. https://doi.org/10.1007/s11357-008-9060-5
67. Robert, K. A. & Bronikowski, A. M. (2010). Evolution of senescence in nature: physiological evolution in populations of garter snake with divergent life histories. *Am. Nat.* 175: 147–159. https://doi.org/10.1086/649595

## K. Animal contest theory and assessment

68. Maynard Smith, J. & Price, G. R. (1973). The logic of animal conflict. *Nature* 246: 15–18. https://doi.org/10.1038/246015a0
69. Maynard Smith, J. & Parker, G. A. (1976). The logic of asymmetric contests. *Anim. Behav.* 24(1): 159–175. https://doi.org/10.1016/S0003-3472(76)80110-8
70. Parker, G. A. (1974). Assessment strategy and the evolution of fighting behaviour. *J. Theor. Biol.* 47(1): 223–243. https://doi.org/10.1016/0022-5193(74)90111-8
71. Parker, G. A. & Rubenstein, D. I. (1981). Role assessment, reserve strategy, and acquisition of information in asymmetric animal conflicts. *Anim. Behav.* 29(1): 221–240. https://doi.org/10.1016/S0003-3472(81)80170-4
72. Hammerstein, P. & Parker, G. A. (1982). The asymmetric war of attrition. *J. Theor. Biol.* 96(4): 647–682. https://doi.org/10.1016/0022-5193(82)90235-1
73. Enquist, M. & Leimar, O. (1983). Evolution of fighting behaviour: decision rules and assessment of relative strength. *J. Theor. Biol.* 102(3): 387–410. https://doi.org/10.1016/0022-5193(83)90376-4
74. Enquist, M. & Leimar, O. (1987). Evolution of fighting behaviour: the effect of variation in resource value. *J. Theor. Biol.* 127(2): 187–205. https://doi.org/10.1016/S0022-5193(87)80130-3 — verified against CrossRef and the Elsevier record. The 185–207 range circulating in secondary lists is wrong.
75. Payne, R. J. H. (1998). Gradually escalating fights and displays: the cumulative assessment model. *Anim. Behav.* 56(3): 651–662. https://doi.org/10.1006/anbe.1998.0835
76. Mesterton-Gibbons, M., Marden, J. H. & Dugatkin, L. A. (1996). On wars of attrition without assessment. *J. Theor. Biol.* 181(1): 65–83. https://doi.org/10.1006/jtbi.1996.0115
77. Arnott, G. & Elwood, R. W. (2009). Assessment of fighting ability in animal contests. *Anim. Behav.* 77(5): 991–1004. https://doi.org/10.1016/j.anbehav.2009.02.010
78. Arnott, G. & Elwood, R. W. (2008). Information gathering and decision making about resource value in animal contests. *Anim. Behav.* 76(3): 529–542. https://doi.org/10.1016/j.anbehav.2008.04.019
79. Pinto, N. S., Palaoro, A. V. & Peixoto, P. E. C. (2019). All by myself? Meta-analysis of animal contests shows stronger support for self than for mutual assessment models. *Biol. Rev.* 94(4): 1430–1442. https://doi.org/10.1111/brv.12509
80. Taylor, P. W. & Elwood, R. W. (2003). The mismeasure of animal contests. *Anim. Behav.* 65(6): 1195–1202. https://doi.org/10.1006/anbe.2003.2169
81. Briffa, M. & Lane, S. M. (2017). The role of skill in animal contests: a neglected component of fighting ability. *Proc. R. Soc. B* 284(1863): 20171596. https://doi.org/10.1098/rspb.2017.1596
82. Hsu, Y., Earley, R. L. & Wolf, L. L. (2006). Modulation of aggressive behaviour by fighting experience: mechanisms and contest outcomes. *Biol. Rev.* 81(1): 33–74. https://doi.org/10.1017/S146479310500686X
83. Briffa, M. & **Sneddon, L. U.** (2007). Physiological constraints on contest behaviour. *Funct. Ecol.* 21(**4**): 627–637. https://doi.org/10.1111/j.1365-2435.2006.01188.x — corrected: two authors, not one; issue 4, not 3. Print 2007, online 15 September 2006; cite 2007.
83a. Briffa, M. (2008). Decisions during fights in the house cricket, *Acheta domesticus*: mutual or self assessment of energy, weapons and size? *Anim. Behav.* 75(3): 1053–1062. https://doi.org/10.1016/j.anbehav.2007.08.016 — a separate paper, sole-authored. Recorded so the two are never conflated.
84. Enquist, M. (1985). Communication during aggressive interactions with particular reference to variation in choice of behaviour. *Anim. Behav.* 33(4): 1152–1161. https://doi.org/10.1016/S0003-3472(85)80175-5
85. Maynard Smith, J. (1980). Why are there so many threat displays? *J. Theor. Biol.* 87: 313–320. https://doi.org/10.1016/0022-5193(80)90310-0

## Citation verification

All three flagged records were checked against CrossRef and the publisher
landing pages in July 2026.

| Record | Outcome |
|---|---|
| 74 Enquist & Leimar 1987 | Confirmed 127(2): 187–205. The competing 185–207 range is a secondary-source error. |
| 55 Leitch & Catania 2012 | Confirmed entirely, DOI and PMC ID both correct. |
| 83 Briffa **& Sneddon** 2007 | Corrected: missing second author, wrong issue. Print 2007, online 2006. |

The Briffa 2007 error is the instructive one. A missing co-author is the kind
of mistake that propagates silently through every downstream document, and it
came from a secondary reference list rather than from the publisher.

Two records still carry a caveat: Huey et al. 1990 (98) predates DOIs and is
stable only via JSTOR, and Baeckens et al. 2017 (97) has an epub date ahead of
its print issue.

## Figures that are design inputs, not constants

Two numbers from this literature are attractive and should be handled with
care. Peregrine visual acuity is estimated at roughly 140 cycles per degree,
the highest documented in any animal; crocodilian integumentary sensory organ
counts run to roughly 4,000 in *A. mississippiensis* and 9,000 in
*C. niloticus*. Both are single-study or small-sample estimates. Use them for
order of magnitude when setting detection ranges. Do not treat them as
calibration targets.

## L. Neck leverage and long-neck predation

86. Kambic, R. E., Roberts, T. J. & Gatesy, S. M. (2017). Experimental determination of three-dimensional cervical joint mobility in the avian neck. *Frontiers in Zoology* 14: 37. https://doi.org/10.1186/s12983-017-0223-z
87. Snively, E. & Russell, A. P. (2007). Functional variation of neck muscles and their relation to feeding style in Tyrannosauridae and other large theropod dinosaurs. *The Anatomical Record* 290(8): 934–957. https://doi.org/10.1002/ar.20563
88. Snively, E. & Russell, A. P. (2007). Craniocervical feeding dynamics of *Tyrannosaurus rex*. *Paleobiology* 33(4): 610–638. https://doi.org/10.1666/06059.1
89. Christian, A., Peng, G., Sekiya, T., Ye, Y., Wulf, M. G. & Steuer, T. (2013). Biomechanical reconstructions and selective advantages of neck poses and feeding strategies of sauropods with the example of *Mamenchisaurus youngi*. *PLOS ONE* 8(10): e71172. https://doi.org/10.1371/journal.pone.0071172
90. McHenry, C. R., Wroe, S., Clausen, P. D., Moreno, K. & Cunningham, E. (2007). Supermodeled sabercat: predatory behavior in *Smilodon fatalis* revealed by high-resolution 3D computer simulation. *PNAS* 104(41): 16010–16015. https://doi.org/10.1073/pnas.0706086104
91. Wroe, S., Chamoli, U., Parr, W. C. H., Clausen, P., Ridgely, R. & Witmer, L. (2013). Comparative biomechanical modeling of metatherian and placental saber-tooths. *PLOS ONE* 8(6): e66888. https://doi.org/10.1371/journal.pone.0066888
92. Brown, J. G. (2014). Jaw function in *Smilodon fatalis*: a reevaluation of the canine shear-bite and a proposal for a new forelimb-powered class 1 lever model. *PLOS ONE* 9(10): e107456. https://doi.org/10.1371/journal.pone.0107456

## M. Attack rates and hunting success

93. Dekker, D. & Drever, M. C. (2016). Interactions of Peregrine Falcons and Dunlin wintering in British Columbia, 1994–2015. *J. Raptor Res.* 50(4): 363–369. https://doi.org/10.3356/JRR-16-21.1
94. Time, B. E. (2016). Hunting activity by urban Peregrine Falcons during autumn and winter in south-west Norway. *Ornis Norvegica* 39: 39–44.

## N. Sensory investment and foraging mode

95. Cooper, W. E., Jr. (2000). An adaptive difference in the relationship between foraging mode and responses to prey chemicals in two congeneric scincid lizards. *Ethology* 106(3): 193–206. https://doi.org/10.1046/j.1439-0310.2000.00523.x
96. Leng, X. et al. (2025). How foraging mode sculpts sensory systems: morphological evidence from DiceCT and histology in sympatric lizards. *Ecology and Evolution* 15: e72042. https://doi.org/10.1002/ece3.72042
97. Baeckens, S., Van Damme, R. & Cooper, W. E., Jr. (2017). How phylogeny and foraging ecology drive the level of chemosensory exploration in lizards and snakes. *J. Evol. Biol.* 30(3): 627–640. https://doi.org/10.1111/jeb.13032

## O. Performance senescence

98. Huey, R. B., Dunham, A. E., Overall, K. L. & Newman, R. A. (1990). Variation in locomotor performance in demographically known populations of the lizard *Sceloporus merriami*. *Physiological Zoology* 63(5): 845–872. Pre-DOI; stable via JSTOR.
99. Erickson, G. M., Lappin, A. K. & Vliet, K. A. (2003). The ontogeny of bite-force performance in American alligator. *J. Zool.* 260(3): 317–327. https://doi.org/10.1017/S0952836903003819 — **ontogenetic, not senescent.** Bite force rising from hatchling to adult. Do not cite for decline.
100. Lailvaux, S. P., Zajitschek, F., Dessman, J. & Brooks, R. (2011). Differential aging of bite and jump performance in virgin and mated *Teleogryllus commodus* crickets. *Evolution* 65(11): 3138–3147. https://doi.org/10.1111/j.1558-5646.2011.01358.x
101. Oufiero, C. E. & Garland, T., Jr. (2009). Repeatability and correlation of swimming performances and size over varying time-scales in the guppy. *Funct. Ecol.* 23(5): 969–978. https://doi.org/10.1111/j.1365-2435.2009.01571.x
102. Hoekstra, L. A., Schwartz, T. S., Sparkman, A. M., Miller, D. A. W. & Bronikowski, A. M. (2020). The untapped potential of reptile biodiversity for understanding how and why animals age. *Funct. Ecol.* 34(1): 38–54. https://doi.org/10.1111/1365-2435.13450

## P. Skill as a graded, separable axis

103. Lane, S. M. & Briffa, M. (2020). The role of spatial accuracy and precision in hermit crab contests. *Anim. Behav.* 167: 111–118. https://doi.org/10.1016/j.anbehav.2020.07.013
104. Loser-effect duration evolves independently of fighting ability (2019). *Proc. R. Soc. B* 286(1904): 20190582. https://doi.org/10.1098/rspb.2019.0582

## G. Design interpretation rules

- Historical dragon labels are inspiration, not a rigid biological taxonomy.
- Body plan determines locomotion and combat opportunity.
- Age affects size, knowledge, injury history, recovery, speed, stamina, and risk tolerance. It is not a level multiplier.
- Experience is stored separately from biological age.
- Size increases reach, momentum, bite potential, durability, and intimidation, and worsens acceleration, turning, heat dissipation, takeoff requirements, and recovery.
- Higgsfield output is anatomy and movement reference until converted into reviewed, rigged, optimised, rights-cleared production assets.
- Unreal MCP is Experimental. Generated editor work is reviewed, tested, and version-controlled.
