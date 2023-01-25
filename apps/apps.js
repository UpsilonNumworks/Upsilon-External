'use strict';

angular.module('nwas').service('apps', function () {
  return [
    {
      name: 'Playground',
      description: { en: 'doiwjdw' },
    },
    {
      name: 'Morse',
      description: {
        en: "Morse sender, receiver and translator (requires RichardDorian's fork)",
        fr: 'Envoyeur, récepteur et interpréteur de morse (nécessite le fork de RichardDorian)',
      },
    },
    {
      name: 'UnitCircle',
      description: { en: 'Unit circle', fr: 'Cercle trigonométrique' },
    },
    {
      name: 'KhiCAS',
      description: {
        en: 'Computer algebra system',
        fr: 'Système de calcul formel',
      },
    },
    {
      name: 'Periodic',
      description: {
        en: 'Periodic table of elements',
        fr: 'Tableau périodique des éléments',
      },
    },
    {
      name: 'Nofrendo',
      description: { en: 'NES emulator', fr: 'Émulateur NES' },
    },
    {
      name: 'Peanut-GB',
      description: { en: 'GameBoy emulator', fr: 'Émulateur GameBoy' },
    },
    {
      name: 'HexEdit',
      description: { en: 'Hexadecimal editor', fr: 'Éditeur hexadécimal' },
    },
    {
      name: 'BadApple',
      description: { en: 'Bad Apple demo', fr: 'Démo Bad Apple' },
    },
    {
      name: 'CHIP-8',
      description: { en: 'CHIP-8 interpreter', fr: 'Interpréteur CHIP-8' },
    },
  ];
});
