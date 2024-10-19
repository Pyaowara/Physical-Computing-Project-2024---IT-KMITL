import { defineConfig } from 'vite';

export default defineConfig({
  define: {
    'process.env': {
      VITE_API_URL: JSON.stringify(process.env.BLYNK_TOKEN),
    },
  },
});
