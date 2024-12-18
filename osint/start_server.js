const { exec } = require('child_process');

// Commande pour démarrer le serveur
const startServerCommand = 'node e:/osint/web_server/app.js';

exec(startServerCommand, (error, stdout, stderr) => {
  if (error) {
    console.error(`Erreur lors du démarrage du serveur: ${error.message}`);
    return;
  }
  if (stderr) {
    console.error(`Erreur de serveur: ${stderr}`);
    return;
  }
  console.log(`Serveur démarré avec succès:\n${stdout}`);
  console.log('Prêt à connecter un appareil pour l\'analyse.');
});
