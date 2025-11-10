let
  pkgs = import (fetchTarball "https://github.com/NixOS/nixpkgs/archive/release-24.05.tar.gz") {};
in
pkgs.mkShell {
  buildInputs = with pkgs; [
    gcc
    clang
    llvm
    gnumake
    pkg-config
    sfml
    xorg.libX11
    libxkbcommon
    ffmpeg            # export futuro de MP4
    gtest             # testes unitários (headers + cmake/pkgconfig)
    gcovr
  ];
  # Variáveis para facilitar localização de libs (se necessário em Makefile futuramente)
  # Usar clang++ para gerar artefatos de cobertura compatíveis com llvm-cov
  CXX = "${pkgs.clang}/bin/clang++";
  shellHook = ''
  echo "Ambiente Nix: C++23 + SFML + ffmpeg + gtest carregado.";
    echo "Use 'make' para compilar e 'make ds_tests' para rodar testes (após target ser adicionado)."
  '';
}
