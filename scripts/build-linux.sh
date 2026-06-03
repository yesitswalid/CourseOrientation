#!/usr/bin/env bash
# =============================================================================
#  build-linux.sh — Compilation + packaging de CourseOrientation (Linux)
#
#  Prérequis :
#    - Qt 6.x installé (qmake dans PATH ou QMAKE_PATH défini)
#    - gcc / g++ 10+, make, cmake
#    - libgl-dev, libudev-dev, libxcb-*-dev
#    - Optionnel : linuxdeployqt pour créer une AppImage autonome
# =============================================================================
set -euo pipefail

BUILD_DIR="build-linux"
DIST_DIR="dist-linux"
BINARY="CourseOrientation"
APPIMAGE_TOOL="linuxdeployqt-continuous-x86_64.AppImage"

# Nombre de cœurs disponibles
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Utiliser QMAKE_PATH si défini, sinon chercher qmake dans PATH
QMAKE="${QMAKE_PATH:-qmake}"

echo ""
echo "======================================================"
echo "  CourseOrientation — Build Linux Release"
echo "======================================================"
echo ""

# ── Créer et configurer le build ─────────────────────────────────────────────
echo "[1/4] Génération du Makefile..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
"$QMAKE" ../CourseOrientation.pro CONFIG+=release
cd ..

# ── Compilation ───────────────────────────────────────────────────────────────
echo "[2/4] Compilation avec $NPROC cœurs..."
make -C "$BUILD_DIR" -j"$NPROC"

# ── Préparer l'AppDir ─────────────────────────────────────────────────────────
echo "[3/4] Préparation de l'AppDir..."
rm -rf "$DIST_DIR" AppDir
mkdir -p AppDir/usr/bin
mkdir -p AppDir/usr/share/applications
mkdir -p AppDir/usr/share/icons/hicolor/256x256/apps

cp "$BUILD_DIR/$BINARY" AppDir/usr/bin/

cat > AppDir/usr/share/applications/CourseOrientation.desktop << 'EOF'
[Desktop Entry]
Name=CourseOrientation
Comment=Gestion de courses d'orientation
Exec=CourseOrientation
Icon=CourseOrientation
Type=Application
Categories=Education;Sports;
EOF

# Copier / convertir l'icône
if [ -f assets/Orienteur.jpg ]; then
    convert assets/Orienteur.jpg \
        AppDir/usr/share/icons/hicolor/256x256/apps/CourseOrientation.png \
        2>/dev/null || cp assets/Orienteur.jpg \
            AppDir/usr/share/icons/hicolor/256x256/apps/CourseOrientation.png
fi

# ── Packager avec linuxdeployqt (si disponible) ───────────────────────────────
echo "[4/4] Packaging..."

if ! command -v "$APPIMAGE_TOOL" &>/dev/null; then
    echo "  → Téléchargement de linuxdeployqt..."
    wget -q "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/$APPIMAGE_TOOL"
    chmod +x "$APPIMAGE_TOOL"
fi

ARCH=x86_64 "./$APPIMAGE_TOOL" \
    AppDir/usr/share/applications/CourseOrientation.desktop \
    -qmake="$QMAKE" \
    -appimage \
    -no-translations \
    -unsupported-allow-new-glibc 2>&1 || true

APPIMAGE=$(ls "${BINARY}"*.AppImage 2>/dev/null | head -1)
if [ -n "$APPIMAGE" ]; then
    mv "$APPIMAGE" "CourseOrientation-linux-x86_64.AppImage"
    echo ""
    echo "======================================================"
    echo "  Build terminé : CourseOrientation-linux-x86_64.AppImage"
    echo "======================================================"
else
    # Fallback : tarball simple si AppImage non disponible
    mkdir -p "$DIST_DIR"
    cp "$BUILD_DIR/$BINARY" "$DIST_DIR/"
    tar czf "CourseOrientation-linux-x86_64.tar.gz" -C "$DIST_DIR" .
    echo ""
    echo "======================================================"
    echo "  Build terminé : CourseOrientation-linux-x86_64.tar.gz"
    echo "======================================================"
fi
echo ""
