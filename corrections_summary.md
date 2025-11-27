# Correções Necessárias para ESP32-S3 Touch AMOLED 1.8

## Problemas Identificados:

### 1. Configurações de Display Incorretas
- **Driver incorreto**: Usando ST7789 em vez de SH8601
- **Tamanho incorreto**: 368x448 em vez de 320x480
- **Pins SPI incorretos**: Precisam ser ajustados para o ESP32-S3 Touch AMOLED

### 2. Configurações de Pinos
- **QSPI pins**: Devem ser 45, 47, 48, 4, 3, 10 (SCLK, MISO, MOSI, CS, DC, RST)
- **I2C pins**: Devem ser 38, 39 (SDA, SCL)
- **Touch CS**: Deve ser definido como -1 (não utilizado)

### 3. Configurações de Build
- **Partitions**: Precisa de esquema de 16MB
- **Build flags**: Precisam de ajustes para o SH8601
- **Libraries**: Precisam de bibliotecas específicas para o display

## Correções Aplicadas:

### ✅ platformio.ini
- Alterado de ST7789_DRIVER para SH8601_DRIVER
- Corrigido tamanho para 320x480
- Mantidos pins QSPI corretos

### ✅ user_setup.h
- Alterado de ST7789_DRIVER para SH8601_DRIVER
- Adicionado tamanho correto 320x480
- Mantidos pins QSPI corretos

### ⚠️ Pendentes:
- Atualizar pin_config.h para remover duplicações
- Adicionar bibliotecas específicas para SH8601
- Implementar inicialização correta do display
- Configurar controle de brilho via QSPI

## Próximos Passos:
1. Testar compilação com as novas configurações
2. Implementar inicialização do display SH8601
3. Adicionar controle de brilho
4. Verificar funcionamento do touch (se aplicável)