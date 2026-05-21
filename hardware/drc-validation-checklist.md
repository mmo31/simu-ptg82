# Checklist DRC/ERC et validation pre-fabrication

## Phase 1 : Vérification schéma (ERC)

- [ ] Tous les nets sont nommés explicitement (pas de "Net1", "Net2", etc.)
  - `GND_SENS` (pin 1 tous les XLR)
  - `+5V_SENS` (pin 2 tous les XLR)
  - `AXIS_X_SIG`, `AXIS_RX_SIG`
  - `ENC_Y_A`, `ENC_Y_B`, `ENC_Z_A`, `ENC_Z_B`
  - `BTN_0_SIG`

- [ ] Pas de net "flottant" (unconnected)

- [ ] Polyfuse F1 en série sur le +5V_SENS avant distribution

- [ ] Résistances R1..R7 (220 Ω) chacune en série sur un signal (A0, A1, D0, D1, D2, D3, D4)

- [ ] Connecteurs XLR J1..J5 assignés correctement :
  - **Pin 1** (GND_SENS) : masse commune
  - **Pin 2** (+5V_SENS) : alimentation commune
  - **Pin 3+** : signaux respectifs (A0, A1, D2, D3, D0, D1, D4)
  - Pour F1 (Axe X) : pin1=GND, pin2=+5V, pin3=A0
  - Pour F2 (Axe RX) : pin1=GND, pin2=+5V, pin3=A1
  - Pour F3 (Encodeur Y) : pin1=GND, pin2=+5V, pin3=D2, pin4=D3
  - Pour F4 (Encodeur Z) : pin1=GND, pin2=+5V, pin3=D0, pin4=D1
  - Pour F5 (Bouton) : pin1=GND, pin2=+5V, pin3=D4

- [ ] Headers Arduino H1 bien placés et orientés (côtés gauche/droit)

---

## Phase 2 : Vérification PCB (DRC)

### Dimensioning
- [ ] Carte 110 x 80 mm (ou approuvée)
- [ ] Épaisseur 1.6 mm
- [ ] Coins arrondis ou légèrement chanfreinés (si souhaité)

### Clearance / Spacing
- [ ] Clearance trace-trace >= 0.2 mm ✓
- [ ] Clearance trace-via >= 0.25 mm ✓
- [ ] Via minimaliste (0.3 mm diam., 0.2 mm anneau) ✓

### Largeur piste
- [ ] +5V >= 0.6 mm ✓
- [ ] GND >= 0.6 mm ✓
- [ ] Signaux >= 0.25 mm ✓

### Pads / Trous
- [ ] Trou soudage XLR >= 0.4 mm ✓
- [ ] Trous fixation M3 (3.2 mm) ou M2.5 (2.1 mm) sans DRC warning ✓

### Plan de masse
- [ ] Plan GND continu sur couche inférieure (ou au moins B.Cu) ✓
- [ ] Pas de "îlots" GND isolés ✓

### Via de masse
- [ ] Vias abondantes (tous les ~1 cm²) entre GND couches ✓

---

## Phase 3 : Routage spécifique

### Alimentation
- [ ] Source H1 5V -> F1 -> distribution (fan-out) -> J1..J5 pin 1
- [ ] Retour masse commun (une seule équipotentielle GND)
- [ ] Pas de boucles de masse parasites

### Signaux
- [ ] Chaque signal route via sa résistance 220 Ω
- [ ] Longueur piste minimale pour chaque signal
- [ ] Pas de croisement inutile (crossing) si possible

### Intégrité signal (optionnel pour shield simple)
- [ ] Pas d'épingles (stubs) sur signaux
- [ ] Pistes retour GND proche des signaux (règle de couplage)

---

## Phase 4 : Documentation fichiers

Avant export Gerber, vérifier présence :

- [ ] Schéma nommé : `XLR_Leonardo_Shield_v1.sch` (ou .kicad_sch pour KiCad)
- [ ] PCB nommé : `XLR_Leonardo_Shield_v1.pcb` (ou .kicad_pcb)
- [ ] BOM export : CSV avec Valeur, Quantité, Désignation
- [ ] Netlist CSV de référence
- [ ] Placement document (PDF ou image)

---

## Phase 5 : Génération Gerber

### Via EasyEDA
1. Menu **File** → **Export** → **Gerber**
2. Sélectionner :
   - Couches à exporter : F.Cu, B.Cu, F.Silkscreen (optionnel), Edge.Cuts
   - Pas de NPTH, pas d'internal planes
3. Format : **RS274X** (standard)
4. Unité : **mm** (compatible JLCPCB/EasyEDA)

### Fichiers générés
- `XLR_Leonardo_Shield.GTL` (top copper)
- `XLR_Leonardo_Shield.GBL` (bottom copper)
- `XLR_Leonardo_Shield.GTS` (top solder mask)
- `XLR_Leonardo_Shield.GBS` (bottom solder mask)
- `XLR_Leonardo_Shield.GTO` (top silk)
- `XLR_Leonardo_Shield.GBO` (bottom silk)
- `XLR_Leonardo_Shield.GKO` (outline / edge cuts)
- `XLR_Leonardo_Shield.TXT` (drill file)

> Zipper tous les fichiers en `.zip` pour envoi à JLCPCB/PCBWay.

---

## Phase 6 : Pre-fabrication checklist JLCPCB / EasyEDA

- [ ] Importation Gerber OK (aperçu à l'écran)
- [ ] Couche top/bottom symétriques et correctes
- [ ] Épaisseur PCB bien sélectionnée (1.6 mm)
- [ ] Copper weight : 1 oz (par défaut)
- [ ] Holes vérifiés (aucun X-out warning)
- [ ] Solder mask color choisi (noir recommandé pour clarté silk)
- [ ] Silk screen lisible (texte >= 0.5 mm)

---

## Phase 7 : Test électrique (post-fabrication)

**Avant soudure composants :**

- [ ] Continuité `+5V_SENS` sur tous XLR pin 1
- [ ] Continuité `GND_SENS` sur tous XLR pin 2
- [ ] Isolation `+5V_SENS` vs `GND_SENS` (>10 MΩ)
- [ ] Continuité piste signaux A0, A1, D0..D4 vers XLR

**Après soudure composants :**

- [ ] Polyfuse non ouvert (continuité)
- [ ] Résistances R1..R7 : ~220 Ω mesurable
- [ ] Headers Arduino : contacts sans court-circuit

**Avec Arduino Leonardo connecté :**

- [ ] Shield reconnu électriquement (aucun court-circuit)
- [ ] Tension +5V stable sur XLR
- [ ] Test logiciel firmware (capteurs détectés)

---

## Notes finales

✅ **Après validation complète**, la carte est prête pour **production en lot** ou **intégration définitive**.

⚠️ **Point critique** : soudure des connecteurs XLR (prise mécanique). Prévoir un **jig de montage** ou **guide gabarit** si production >5 unités.

🔧 **Support** : en cas de warning DRC/ERC, consulter [xlr-shield-plan.md](xlr-shield-plan.md) et [pcb-placement-guide.md](pcb-placement-guide.md).
