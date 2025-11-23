'use strict';

angular.module('nwas').service('apps', function() {
  return [
    { name: "KhiCAS", description: {en: "Computer algebra system", fr: "Système de calcul formel"} },
    { name: "Periodic", description: {en: "Periodic table of elements", fr: "Tableau périodique des éléments"} },
    { name: "Nofrendo", description: {en: "NES emulator", fr: "Émulateur NES"} },
    { name: "Peanut-GBC", description: {en: "GameBoy Color emulator", fr: "Émulateur GameBoy Color"} },
    { name: "Peanut-GB", description: {en: "GameBoy emulator", fr: "Émulateur GameBoy"} },
    { name: "HexEdit", description: {en: "Hexadecimal editor", fr: "Éditeur hexadécimal"} },
    { name: "BadApple", description: {en: "Bad Apple demo", fr: "Démo Bad Apple"} },
    { name: "UnitCircle", description: {en: "Unit circle", fr: "Cercle trigonométrique"} },
    { name: "CHIP-8", description: {en: "CHIP-8 interpreter", fr: "Interpréteur CHIP-8"} },
    { name: "Flash2Ram", description: {en: "Copy files from flash to RAM", fr: "Copiez des fichiers de la flash vers la RAM"} },
    { name: "PngBrowser", description: {en: "View PNG images", fr: "Affichez des images PNG"} },
    { name: "RayCaster", description: {en: "A simple raycasting project", fr: "Un simple projet de raycasting"} },
    { name: "Compressor", description: {en: "Compress your files", fr: "Compressez vos fichiers"} },
    { name: "Backup", description: {en: "Backup scripts wirelessly", fr: "Sauvegardez vos fichiers sans fil"} },
    { name: "FlappyBird", description: {en: "A Flappy Bird clone", fr: "Un clone de Flappy Bird"} },
    { name: "HoldemPoker", description: {en: "A Texas Hold'em Poker Game", fr: "Un jeu de Texas Hold'em"} },
  ];
});
