# Repositorio de TP 2 del grupo 2 - Sistemas Operativos de Tiempo Real (II)

## 1. Objetivo
Desarrollar un programa en RTOS que resuelva el problema planteado en el [enunciado de trabajo práctico 2](https://docs.google.com/document/d/1i_nvWkR8Uy2a3vuZ9dQ0bey6un-p7EYcWo7U27207WE/edit?tab=t.0#heading=h.8jl34vsv9f2z), usando como plantilla el desarrollo de trabajo práctico 1 (objetos reactivos) y aplicando lo aprendido en el curso sobre **memoria dinámica**.

## 2. Integrantes de Grupo 2

- `e2405` -- Jezabel Victoria Danon.
- `e2406` -- Mariano Ariel Deville.
- `ext161` -- Victor Manuel Canepa.

## 3. Docentes revisores

- Juan Manuel Cruz.
- Sebastian Bedin.

<!-- ## 4. Archivos soporte:
[link al video](https://drive.google.com/file/d/13P8zk9Db0W95AT1wKOAUxqjPOFZNQk1W/view?usp=sharing)
-->
---

## 4. Breve guía para trabajo colaborativo (organización del repositorio git)

- **Uso de ramas (`branches`) para evitar pisar cambios entre integrantes:**
	- **Rama principal [`main`]:** En este _branch_ (rama) solo queda el código **revisado y estable**.
	- **Ramas de desarrollo [`dev_nombre`]:** Cada integrante puede trabajar libremente en su propia rama y cuando le convenga hacer _merge_ con la rama `main`.

### 4.1 Guía rápida:

- **CREAR TU RAMA DE TRABAJO (solo se hace una vez)**:

  - Ejecutá los siguientes comandos `git` en el repositorio:
    ```bash
      git checkout main	            # Te asegura que estás parado en el branch principal (main).
      git checkout -b dev_NOMBRE    # Desde main, el usuario "NOMBRE" crea una rama para desarrollo (dev).
      git push origin dev_NOMBRE    # La nueva rama se sube a github (desde el remoto se puede ver tanto main como dev_NOMBRE).
    ```
  
- **SUBIR CAMBIOS DESDE TU RAMA DE TRABAJO**

  Cada uno puede trabajar desde su rama (sin preocuparse de hacer subidas con riesgo de romper la versión revisada y estable):

    - Cada día, al comenzar, haces lo siguiente:
      ```bash
      git branch	 # Te dice en que rama estás trabajando actualmente (siempre debería ser la tuya evitando cambios en main).
      git checkout dev_NOMBRE	 # Si por accidente estás en otra rama, esto te asegura que ahora estás parado en tu branch de desarrollo (dev_NOMBRE).
      # trabajar normalmente
      git add .
      git commit -m "tu mensaje."
      git push       # aca subis tus cambios y commit a github pero ahora con el alivio de que no estas modificando la rama main.
      ```
    - Además, **mientras trabajas en tu rama**, si te interesa ver cómo viene quedando la rama `main` (que será el proyecto **entregable**), o las ramas de tus compañeros, podés ver todo muy fácil desde Github.

- **ACTUALIZAR RAMA `main` CON TUS CAMBIOS**

  Cada vez que uno de los integrantes cree que tiene algo funcionando o para compartir con el resto:

	- Avisar a los demás integrantes que está **listo para hacer un _Merge_** desde `dev_NOMBRE` a `main`. Esto se puede hacer muy fácil:
		
		- **En Github** seleccionás tu rama (`dev_NOMBRE`) y apretás en el botón **Open pull request**. De esta manera el resto queda enterado que hay pendiente un _merge_ a la rama `main`. 

		- **Observaciones:** Una **buena práctica** y simple es que un integrante (quien lo solicita) inicia el _merge_ y el resto de los integrantes (al menos uno) durante ese proceso revisa los cambios que irán a `main`. Esto asegura que en `main` se carga una versión **verificada y estable** para todos. Todo esto está resuelto usando el botón **Open pull request**.

	---

- **ACTUALIZAR TU RAMA DESDE `main` (solo cuando sea necesario)**

  Este es el mismo proceso _merge_ pero en dirección contraria: desde `main` a tu rama personal (`dev_NOMBRE`). De esta manera cuando se actualiza el `main` y vos estás listo para incorporar los cambios en tu versión de desarrollo, lo haces:
    ```bash
    git checkout dev_NOMBRE
    git pull origin main      # esto trae a tu repositorio local lo nuevo de main.
    ```
	---
