# Repositorio de TP 2 del grupo 2 - Sistemas Operativos de Tiempo Real (II)

## 1. Objetivo
Desarrollar un programa en RTOS que resuelva el problema planteado en el [enunciado de trabajo práctico 2](https://docs.google.com/document/d/1KQhBwUzqrjm5CotFv7kWWn0KHQ8jBhLotwkvgrg0q_s/edit?tab=t.0#heading=h.8jl34vsv9f2z), usando como plantilla el desarrollo de trabajo práctico 1 (objetos reactivos) y aplicando lo aprendido en el curso sobre **memoria dinámica**.

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
	- **Ramas de desarrollo [`dev_nombre`]:** Cada integrante puede trabajar libremente en su propia rama. Luego, cuando decide hacer pull a la rama `main` lo hace con _pull request_: es un pull que inicia el usuario pero no se completa hasta que el resto del equipo no se sincroniza).

### 4.1 Pasos a seguir:

- **CREA TU RAMA DE TRABAJO (solo se hace una vez)**:

  - Ejecutá los siguientes comandos `git` en el repositorio:
    ```bash
      git checkout main	            # Te aseguras que estás parado en el branch principal (main).
      git checkout -b dev_NOMBRE    # Desde main, el usuario "NOMBRE" crea una rama para desarrollo (dev).
      git push origin dev_NOMBRE    # La nueva rama se sube a github.
    ```

- **SUBIR CAMBIOS DESDE TU RAMA DE TRABAJO**

  Cada uno puede trabajar desde su rama (sin preocuparse de hacer subidas con riesgo de romper la versión revisada y estable):

    - Cada día, al comenzar, haces lo siguiente:
      ```bash
      git branch	 # Te dice en que rama estás trabajando actualmente (siempre debería ser la tuya evitando cambios en main).
      git checkout dev_NOMBRE	 # Si por accidente estás en otra rama, esto te asegura que ahora estás parado en tu branch de desarrollo (dev_NOMBRE).
      # trabajar normalmente y luego subir a github...
      git add .
      git commit -m "tu mensaje que sube a github."
      git push       # aca subis tus cambios y commit a github, pero con el alivio de que no estas modificando la rama main.
      ```
      
- **PASAR TUS CAMBIOS A LA RAMA COMÚN (`main`) **

  Cuando tengas mejoras significativas que ya se pueden agregar a la rama `main`:
  
  - en github seleccionas la vista de tu rama (`dev_NOMBRE`) y apretás en el botón **Open pull request**.

  - Hecho esto el resto de los integrantes se entera que hay pendiente un _merge_ de tu rama a la rama `main`. Se avisa a los demás y alguno de ellos sigue en github los pasos dentro de _pull request_, hasta que queda hecho el _merge_ a `main`.
  - 
	---

- **TRAER LOS CAMBIOS DE RAMA COMÚN `main` A TU RAMA DE DESARROLLO**

  Este es el mismo proceso de antes pero en dirección contraria: desde `main` a tu rama personal (`dev_NOMBRE`). De esta manera cuando se actualiza el `main` y vos estás listo para incorporar los cambios en tu versión de desarrollo, lo haces:
    ```bash
    git checkout dev_NOMBRE
    git pull origin main      # esto trae a tu repositorio local lo nuevo de main.
    ```
	---
