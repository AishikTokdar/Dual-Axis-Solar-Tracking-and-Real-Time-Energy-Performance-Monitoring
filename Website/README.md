# Project website

The static presentation site lives under `dist/` and is separate from the live device dashboard in `Dashboard/`.

Preview locally from the repository root:

```powershell
python -m http.server 4173 --directory Website/dist
```

Then open `http://127.0.0.1:4173`.

## Publication

GitHub Actions workflow [`.github/workflows/deploy-website.yml`](../.github/workflows/deploy-website.yml) automatically deploys `Website/dist` to GitHub Pages on pushes to `main` (or via manual `workflow_dispatch`).
