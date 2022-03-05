local GameState = {}

GameState.GameState = {
   STOP = 1,
   STARTING = 2,
   PLAYING = 3,
   PAUSED = 4,
}

gameState = GameState.GameState.STARTING

function GameState.GetGameState()
	return gameState
end

function GameState.SayHi()
    print("Hello World!")
end

function Update(dt)
	gameState = GameState.GameState.PAUSED
end

return GameState --.gameState
