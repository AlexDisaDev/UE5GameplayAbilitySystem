Gameplay Ability System (GAS)
Este proyecto utiliza el Gameplay Ability System (GAS) de Unreal Engine 5.XX para gestionar el Enhanced input system y las habilidades, atributos y efectos del juego. A continuación, se detalla cómo se estructura y se integra este sistema en el código, comenzando desde el Player Controller hasta un Character específico.

Descripción del Sistema GAS
El sistema GAS está diseñado para manejar las habilidades, atributos y efectos dentro del juego. En este proyecto, la lógica del GAS se ha implementado de forma centralizada en el Player Controller, lo que permite la gestión de las habilidades y atributos de manera más flexible y accesible a través del control de un Character específico.

Componentes Clave:
ProjectName.h
Add = 
#pragma once

#include "CoreMinimal.h"
UENUM(BlueprintType)
enum class EGA_AbilityInputID : uint8
{
	// 0 none
	None UMETA(DisplayName = "None"),
	// 1 confirm
	Confirm UMETA(DisplayName = "Confirm"),
	// 2 cancel
	Cancel UMETA(DisplayName = "Cancel"),
	// 3 Ability1 
	Ability1 UMETA(DisplayName = "Ability1"),
	// 4 Ability2
	Ability2 UMETA(DisplayName = "Ability2"),
	// 7 ability 3
	Ability3 UMETA(DisplayName = "Ability3"),
	// 6 Jump
	Ability4 UMETA(DisplayName = "Ability4"),
	//7 
	Ability5 UMETA(DisplayName = "Ability5"),
};



Gas Player Controller (PC)

El Player Controller se encarga de manejar la inicialización y gestión de las habilidades y atributos del jugador.
Dentro del controlador, se gestionan las habilidades activadas, el seguimiento del estado de la habilidad y la activación de eventos relacionados.
El controlador también se comunica con el Character para activar o desactivar las habilidades de acuerdo a las condiciones del juego (por ejemplo, si el personaje está saltando, escalando, o en otro estado específico).
Character Específico

Cada jugador tiene un Character específico que está asociado con un conjunto de Habilidades y Atributos.
El Character contiene el componente AbilitySystemComponent, que maneja las habilidades activadas, y el AttributeSet, que gestiona los atributos como salud, energía, etc.
Se asegura que las habilidades solo puedan activarse si el Character está en un estado válido (por ejemplo, no puede ejecutar habilidades mientras está escalando).
Gameplay Ability (GA)

Las habilidades se implementan como clases de Gameplay Ability que pueden ser activadas por el jugador.
Cada habilidad tiene un conjunto de efectos asociados y se activa mediante acciones específicas en el juego, como presionar un botón o al completar una secuencia de animación.
Las habilidades son gestionadas por el AbilitySystemComponent, que se asegura de que solo se activen en condiciones apropiadas.
Gestión de Estados del Personaje

El sistema verifica constantemente el estado del personaje (por ejemplo, si está saltando, escalando o caído) antes de permitir la activación de nuevas habilidades.
Se utiliza un CharacterMovementComponent para obtener información sobre el estado de movimiento del personaje y determinar si ciertas habilidades están bloqueadas.
Integración en el Juego
El flujo básico de trabajo en el sistema es el siguiente:

Inicialización: Al iniciar el juego, el Player Controller se encarga de crear y asignar las capacidades y atributos correspondientes al Character.
Activación de Habilidades: Cuando un jugador intenta activar una habilidad (por ejemplo, presionando un botón), el Player Controller verifica si el Character está en un estado válido (por ejemplo, no está escalando o en el aire).
Bloqueo de Habilidades: Algunas habilidades pueden ser bloqueadas dependiendo del estado del personaje, utilizando el componente CharacterMovementComponent para determinar si el personaje está realizando una acción incompatible con la habilidad.
Efectos y Animaciones: Si la habilidad se activa correctamente, se ejecutan sus efectos (por ejemplo, causar daño, curar, etc.) y se reproduce una animación asociada a la habilidad.
Finalización: Después de que la habilidad se ha ejecutado o ha sido cancelada, se limpia el estado y el AbilitySystemComponent se actualiza para reflejar los cambios.
¿Cómo Funciona?
Asignación de Habilidades: Las habilidades se asignan a los jugadores desde el Player Controller.
Activación de la Habilidad: Cuando el jugador presiona el botón correspondiente, el Player Controller verifica las condiciones (como si el personaje está saltando, escalando, etc.) y activa la habilidad si es válida.
Desactivación de la Habilidad: Si el personaje entra en un estado que no permite la activación de habilidades, estas son bloqueadas hasta que el personaje regrese a un estado válido.

Escalabilidad: Puedes agregar fácilmente nuevas habilidades y efectos al sistema sin tener que modificar demasiado la lógica central.