# 🔬 Physics Simulation Platform

Uma plataforma web interativa para visualização de fenômenos físicos com **alto rigor matemático** e **experiência moderna**, integrando um motor de cálculo em C++ com interface web em React.

---

## 📌 Sobre o Projeto

O ensino de Física nas Engenharias frequentemente enfrenta limitações em ferramentas disponíveis:

* Simuladores web (JavaScript) → limitados em precisão e performance
* Softwares profissionais como MATLAB → poderosos, porém pouco acessíveis e com curva de aprendizado elevada

Este projeto propõe uma solução híbrida:

> 💡 **Unir desempenho de baixo nível (C++) com acessibilidade web (React)**

---

## 🎯 Objetivo

Desenvolver uma plataforma que permita:

* Visualizar fenômenos físicos em tempo real
* Manipular parâmetros dinamicamente (sliders, inputs)
* Garantir precisão matemática através de métodos numéricos robustos
* Tornar o aprendizado mais intuitivo e interativo

---

## 🏗️ Arquitetura do Sistema

O sistema segue uma arquitetura desacoplada em três camadas:

```
Frontend (React)
        ↓
Middleware (Node.js)
        ↓
Motor de Física (C++)
```

### 🔹 Frontend (React.js)

* Interface moderna e responsiva
* Interação em tempo real com o usuário
* Renderização de gráficos e simulações

### 🔹 Middleware (Node.js)

* Comunicação entre frontend e backend
* Gerenciamento de requisições
* Integração com o motor nativo

### 🔹 Engine (C++)

* Núcleo computacional do sistema
* Resolução de equações diferenciais
* Processamento de simulações físicas

---

## ⚙️ Tecnologias Utilizadas

* **Frontend:** React.js, HTML5, CSS3
* **Backend:** Node.js
* **Engine:** C++
* **Visualização:** Canvas API / Recharts
* **Integração:** JSON / APIs REST

---

## 🧪 Módulos de Simulação (MVP)

### 📍 Física 1 — Cinemática e Dinâmica

* Movimento de projéteis
* Resistência do ar
* Colisões com conservação de momento

### 🌊 Física 2 — Ondulatória

* Superposição de ondas
* Interferência
* Ajuste de frequência e fase

### ⚡ Física 3 — Eletromagnetismo *(planejado)*

* Linhas de campo elétrico
* Potencial elétrico
* Distribuições de carga

---

## 🧠 Diferenciais do Projeto

* 🔬 Separação entre lógica física e interface
* ⚡ Uso de C++ para alta performance
* 🌐 Acessibilidade via navegador
* 📊 Visualização interativa de dados
* 🧩 Arquitetura modular e escalável

---

## 📈 Roadmap

* [ ] Implementação do motor de física em C++
* [ ] Integração com Node.js
* [ ] Desenvolvimento da interface em React
* [ ] Criação do módulo de Física 1
* [ ] Adição de módulos avançados
* [ ] Testes e validação dos resultados

---

## 🚀 Possíveis Expansões

* Compilação do motor para WebAssembly (WASM)
* Modo educacional com explicações teóricas
* Exportação de dados (CSV/JSON)
* Comparação de performance (JS vs C++)

---

## 👨‍💻 Equipe

* Eric Kamakawa
* Felipe Rochoel
* Iago Sitta

---

## 📄 Licença

Este projeto está sob a licença MIT.

---

## 📚 Contexto Acadêmico

Projeto desenvolvido para a disciplina de **Oficina de Integração**, com foco na aplicação prática de conceitos de:

* Engenharia de Software
* Computação Científica
* Desenvolvimento Web
* Métodos Numéricos

---

## 💡 Visão

> Transformar o aprendizado de Física em uma experiência interativa, precisa e acessível para estudantes de engenharia.
