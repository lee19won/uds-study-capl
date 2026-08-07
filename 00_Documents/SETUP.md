# CANoe 설정 만들기 (최초 1회)

이 저장소는 **직접 작성한 소스(CAPL / DBC / Panel)만** 관리합니다.
CANoe 설정 파일(`.cfg`)은 각자 PC에서 직접 만들어 사용합니다.

> **왜 .cfg 를 공유하지 않나요?**
> - CANoe 버전이 다르면 열리지 않습니다 (상위 → 하위 불가)
> - 내부에 절대 경로가 저장되어 다른 PC 에서 파일을 못 찾습니다
> - 바이너리라 두 사람이 수정하면 병합이 불가능합니다
> - 설정을 직접 만들어보는 것 자체가 좋은 학습입니다

소요 시간: **약 5분**

---

## 준비물

- CANoe **16 SP2 이상** (Help → About 에서 확인)
- 이 저장소를 clone 한 폴더 (예: `C:\study\uds`)
- **CAN 하드웨어 불필요** — Simulated Bus 로 동작합니다

---

## 1. 새 Configuration 생성

1. CANoe 실행
2. **File → New Configuration**
3. 템플릿 선택 창에서 **CAN → CANFD 500kBaud 2MBaud** 선택
   - CAN FD 템플릿이 없으면 일반 **CAN 500kBaud** 로도 됩니다.
     진단 메시지는 Classic CAN 8byte 로 동작하므로 실습에 지장 없습니다.

---

## 2. 저장 위치 지정 (중요)

**File → Save Configuration As**

저장 위치: **clone 한 폴더 안**
```
C:\study\uds\MyConfig\UDS_Study.cfg
```

> `MyConfig` 폴더는 `.gitignore` 에 등록되어 있어 Git 에 올라가지 않습니다.
> 저장소 **바깥**에 저장하면 상대 경로가 성립하지 않으니 반드시 안쪽에 저장하세요.

지금 저장해두어야 이후 단계에서 상대 경로가 올바르게 잡힙니다.

---

## 3. 데이터베이스 연결

1. **Simulation Setup** 창 열기
2. 좌측 네트워크(CAN1) 의 **Databases** 우클릭 → **Add**
3. 파일 선택:
   ```
   ..\DBC\SBCM_XX_CANFD_DataBase_v0_1.dbc
   ```
4. 추가 후 노드 목록에 **SBCM**, **OBD** 가 보이면 성공

---

## 4. CAPL 노드 추가

1. Simulation Setup 에서 CAN1 버스에 **네트워크 노드 추가**
   (빈 공간 우클릭 → Insert Network Node)
2. 노드 이름: `SBCM_Simulator`
3. 노드 우클릭 → **Configuration** → CAPL 파일 지정:
   ```
   ..\CAPL\SBCM_ECU_Simulator.can
   ```

> `NetworkLayer_API.cin` 은 `.can` 파일 안에서 `#include` 로 자동 참조되므로
> 별도로 등록하지 않습니다.

---

## 5. Simulated Bus 모드 활성화

하드웨어 없이 동작시키기 위해 필수입니다.

- 툴바의 **Simulated Bus** 버튼 클릭 (아이콘이 활성화 상태가 되어야 함)
- 또는 **Hardware → Network Hardware Configuration** 에서 Simulated 로 설정

---

## 6. 컴파일 및 실행 확인

1. **Ctrl + F7** (Compile All)
   - Write 창에 오류가 없으면 성공
2. **F9** 또는 Start 버튼으로 측정 시작
3. **Write 창**에 아래 메시지가 나오면 정상입니다:
   ```
   SBCM ECU Simulator: Network Layer 초기화 완료. 0x7B0 요청 대기 중...
   ```

---

## 7. 동작 테스트

**IG (Interactive Generator)** 로 진단 요청을 보내봅니다.

1. 측정 중지
2. **Home → Interactive Generator** 추가
3. 메시지 전송 설정:
   - ID: `0x7B0`
   - DLC: `8`
   - Data: `02 10 01 55 55 55 55 55`
     (Single Frame, 길이 2, 서비스 0x10 세션제어, 서브펑션 0x01)
4. 측정 시작 후 전송

**기대 결과** — Trace 창에 `0x7B8` 응답이 나타납니다.
현재는 Application Layer 가 미구현 상태라 부정 응답이 옵니다:
```
03 7F 10 11 55 55 55 55
```
- `7F` : Negative Response
- `10` : 요청한 서비스
- `11` : serviceNotSupported

여기까지 확인되면 **Network Layer 가 정상 동작**하는 것입니다.

---

## 문제 해결

| 증상 | 조치 |
|---|---|
| DBC 파일을 못 찾음 | 저장소 전체를 clone 했는지 확인. `.cfg` 를 저장소 안에 저장했는지 확인 |
| CAPL 컴파일 오류 | Write 창의 오류 줄 번호 확인. include 경로가 맞는지 확인 |
| 한글 주석 깨짐 | CAPL 파일 첫 줄이 `/*@!Encoding:949*/` 인지, 파일이 CP949 로 저장되었는지 확인 |
| 측정 시작이 안 됨 | Simulated Bus 모드가 켜져 있는지 확인 |
| 응답이 안 옴 | Trace 창에서 0x7B0 이 실제로 송신되었는지 먼저 확인 |

---

## 코드 업데이트 받기

CAPL 소스가 갱신되면:

```
git pull origin main
```

`.cfg` 는 각자 파일이므로 그대로 유지되고, CAPL 파일만 갱신됩니다.
**CANoe 에서 Ctrl+F7 로 재컴파일**하면 최신 코드가 반영됩니다.
