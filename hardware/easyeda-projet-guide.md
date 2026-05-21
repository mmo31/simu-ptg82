# Création du projet EasyEDA (à partir des fichiers du dépôt)

## Ce qui est déjà prêt
- Plan fonctionnel : [xlr-shield-plan.md](xlr-shield-plan.md)
- Netlist de câblage : [xlr-shield-netlist.csv](xlr-shield-netlist.csv)

## Convention de câblage (homogène pour tous les faisceaux)

**Chaque capteur = 1 câble XLR 5 broches :**
- **Pin 1 = GND** (masse/blindage)
- **Pin 2 = +5V** (alimentation)
- **Pin 3, 4, ... = Signaux** (1 signal pour pot/bouton, 2 signaux pour encodeur)

## Étapes (EasyEDA)
1. Créer un nouveau projet : `XLR_Leonardo_Shield`.
2. Créer le schéma et placer :
   - 5 connecteurs XLR 5 broches (`J1..J5`)
   - 1 connecteur Arduino Leonardo shield (headers)
   - 1 polyfuse 500 mA (`F1`)
   - 7 résistances série 220 Ω (`R1..R7`) sur les lignes signaux
3. Nommer les nets exactement :
   - `GND_SENS` (pin 1 de tous les XLR)
   - `+5V_SENS` (pin 2 de tous les XLR)
   - `AXIS_X_SIG`, `AXIS_RX_SIG`
   - `ENC_Y_A`, `ENC_Y_B`, `ENC_Z_A`, `ENC_Z_B`
   - `BTN_0_SIG`
4. Câbler selon la netlist CSV.
5. Convertir en PCB et choisir les empreintes mécaniques XLR.
6. Placer les XLR sur un bord de carte + ajouter trous de fixation.
7. Router selon règles :
   - `+5V/GND` >= 0.6 mm
   - signaux >= 0.25 mm
8. Lancer DRC/ERC puis générer Gerber.

## Mapping Arduino final
- `A0` -> `J1-3`
- `A1` -> `J2-3`
- `D2` -> `J3-3`
- `D3` -> `J3-4`
- `D0` -> `J4-3`
- `D1` -> `J4-4`
- `D4` -> `J5-3`
- `GND` -> `Jx-1` (pin 1 tous les XLR)
- `+5V` -> `Jx-2` (pin 2 tous les XLR)

## Important
- Aucune pull-up externe sur le shield.
- Pas de condensateurs de filtrage sur le shield.
- Les pull-up internes de la carte Arduino sont utilisées par le firmware.

---

## Ressources complémentaires

- [bom-lcsc.md](bom-lcsc.md) : BOM détaillée avec références LCSC/EasyEDA
- [pcb-placement-guide.md](pcb-placement-guide.md) : Plan de placement PCB, dimensionning, routage
- [drc-validation-checklist.md](drc-validation-checklist.md) : Checklist DRC/ERC et validation pre-fabrication
