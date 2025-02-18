# DERIV_SHB2

파생상품 가격결정 및 위험관리를 위한 라이브러리입니다.

## 프로젝트 구조

- `SingleOption/`: 단일 옵션 가격결정 모듈
  - EuropeanOption
  - AmericanOption 
  - AsianOption
  - AutocallOption
  - BarrierOption

- `EuroVanilla_lib/`: 유러피안 옵션 DLL 라이브러리
- `SingleAutocallable_lib/`: Autocallable 구조화상품 DLL 라이브러리
- `Test_stl/`: STL 테스트 모듈

## 주요 기능

- 옵션 가격결정
  - Black-Scholes 모형
  - Monte Carlo 시뮬레이션
  - 유한차분법(FDM)

- Greeks 계산
  - Delta
  - Gamma  
  - Vega
  - Theta

- 변동성 모델링
  - Local Volatility
  - Implied Volatility Surface
