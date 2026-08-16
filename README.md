# uds-study-capl

UDS(ISO 14229) 학습용 CAPL 기반 ECU 시뮬레이터입니다.
실제 제어기 없이 CANoe 만으로 진단 통신을 실습할 수 있습니다.

---

## 관리 범위

이 저장소는 **사용자가 직접 작성한 소스만** 관리합니다.

| 관리함 | 관리하지 않음 |
|---|---|
| CAPL 소스 (`.can`, `.cin`) | CANoe 설정 (`.cfg`) |
| 네트워크 DB (`.dbc`) | 컴파일 결과 (`.cbf`) |
| 패널 (`.xvp`) | 측정 로그 (`.blf`, `.asc`, `.log`) |
| 문서 (`.md`) | 리포트, 백업, 임시 파일 |

CANoe 설정 파일은 버전 종속·절대 경로·병합 불가 문제가 있어 각자 생성합니다.
설정 방법은 **[Doc/SETUP.md](Doc/SETUP.md)** 를 참고하세요. (약 5분 소요)

---

## 시작하기

### 1. 사전 준비
- **CANoe 16 SP2 이상**
- **Git** : https://git-scm.com/download/win

### 2. 저장소 받기
```
git clone https://github.com/lee19won/uds-study-capl.git
```

> 경로에 **한글·공백이 없는 곳**에 받으세요. (권장: `C:\study\uds`)
> OneDrive / Google Drive 동기화 폴더는 피하세요. Git 저장소가 손상될 수 있습니다.

### 3. CANoe 설정 만들기
**[Doc/SETUP.md](Doc/SETUP.md)** 를 따라 진행하세요.

### 4. 코드 업데이트
```
git pull origin main
```
CANoe 에서 **Ctrl+F7** 로 재컴파일하면 반영됩니다.

---

## 폴더 구조

```
uds-study-capl/
├─ CAPL/      CAPL 소스
├─ DBC/       네트워크 데이터베이스
├─ Panel/     CANoe 패널
├─ Doc/       설정 가이드 및 스터디 자료
└─ .vscode/   VS Code 공통 설정 (인코딩 등)
```

---

## 진단 사양

| 항목 | 값 |
|---|---|
| 물리 요청 (Tester → SBCM) | **0x7B0** |
| 물리 응답 (SBCM → Tester) | **0x7B8** |
| 전송 프로토콜 | ISO 15765-2 (ISO-TP) |
| 프레임 | Classic CAN 8 byte |
| 패딩 값 | 0x55 |

### 구현 구조

```
[ Network Layer ]  CAPL/NetworkLayer_API.cin
      ISO-TP 분할/조립 (SF / FF / CF / FC)
      N_Bs, N_Cr 타임아웃 처리
              |
              | AppLayer_OnDiagRequestReceived()
              v
[ Application Layer ]  CAPL/SBCM_ECU_Simulator.can
      UDS 서비스 처리 (0x10, 0x22, 0x19 ...)
              |
              | NetworkLayer_SendResponse()
              v
         응답 전송 (자동 분할)
```

---

## CAPL 수정 시 주의사항

### 인코딩 (중요)
CAPL 파일은 **CP949** 로 저장해야 CANoe 에서 한글 주석이 깨지지 않습니다.

VS Code 에서 한글이 깨져 보인다면:
1. 하단 상태바의 인코딩 표시(`UTF-8`) 클릭
2. **Reopen with Encoding** → **Korean (CP949)**

저장 시에도 **Save with Encoding → Korean (CP949)** 를 사용하세요.

> 저장소에 `.gitattributes` 설정이 되어 있어 GitHub 웹에서는 한글이 정상 표시됩니다.

### 커밋 전 확인
`.gitignore` 로 걸러지지만, 변경 목록에 아래 파일이 보이면 커밋하지 마세요.
- `*.cfg` (개인 설정)
- `*.cbf` (컴파일 결과)
- `*.blf`, `*.asc`, `*.log` (측정 로그)

---

## 기여 방법

1. `git pull origin main` 으로 최신 코드 받기
2. 코드 수정 후 CANoe 에서 동작 확인
3. **변경 내용을 눈으로 검토** 후 커밋
4. Push

커밋 메시지 규칙:
```
feat:  새 기능 추가
fix:   버그 수정
docs:  문서 수정
chore: 설정 / 잡무
```

---

## 진행 계획

- [x] Network Layer (ISO-TP) 구현
- [x] 0x10 DiagnosticSessionControl
- [x] 0x3E TesterPresent (S3 타이머)
- [x] 0x22 ReadDataByIdentifier
- [ ] 0x27 SecurityAccess
- [ ] 0x19 ReadDTCInformation
- [ ] 0x2E WriteDataByIdentifier
