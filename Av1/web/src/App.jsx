import { useEffect, useMemo, useRef, useState } from 'react'
import createModule from './wasm/astar.js'
import { DEFAULT_GRID } from './defaultGrid.js'
import './App.css'

const ROWS = 20
const COLS = 20
const WALL = 5
const PATH = 3

const toTask = ([r, c]) => [ROWS - 1 - r, c]
const fmt = (p) => `(${toTask(p).join(', ')})`
const same = (a, b) => a[0] === b[0] && a[1] === b[1]
const key = ([r, c]) => r * COLS + c

const DEFAULT_START = [ROWS - 1, 0]
const DEFAULT_END = [0, COLS - 1]

const cloneGrid = (g) => g.map((row) => [...row])
const emptyGrid = () => Array.from({ length: ROWS }, () => Array(COLS).fill(0))

function randomGrid(start, end, density) {
  const g = emptyGrid()
  for (let r = 0; r < ROWS; r++)
    for (let c = 0; c < COLS; c++)
      if (Math.random() < density && !same([r, c], start) && !same([r, c], end)) g[r][c] = WALL
  return g
}

export default function App() {
  const [wasm, setWasm] = useState(null)
  const [wasmError, setWasmError] = useState(null)
  const [grid, setGrid] = useState(() => cloneGrid(DEFAULT_GRID))
  const [start, setStart] = useState(DEFAULT_START)
  const [end, setEnd] = useState(DEFAULT_END)
  const [tool, setTool] = useState('wall')
  const [result, setResult] = useState(null) // { path, visited }
  const [step, setStep] = useState(0)
  const [playing, setPlaying] = useState(false)
  const [delay, setDelay] = useState(25)
  const [showValues, setShowValues] = useState(false)
  const [hover, setHover] = useState(null)
  const paint = useRef(null) // wartosc malowana przy przeciaganiu myszka

  useEffect(() => {
    createModule().then(setWasm).catch((e) => setWasmError(String(e)))
  }, [])

  const totalFrames = result ? result.visited.length + result.path.length : 0

  useEffect(() => {
    if (!playing) return
    if (step >= totalFrames) {
      setPlaying(false)
      return
    }
    const t = setTimeout(() => setStep((s) => s + 1), delay)
    return () => clearTimeout(t)
  }, [playing, step, totalFrames, delay])

  useEffect(() => {
    const up = () => (paint.current = null)
    window.addEventListener('pointerup', up)
    return () => window.removeEventListener('pointerup', up)
  }, [])

  // Stan kazdej komorki przy obecnym kroku animacji
  const view = useMemo(() => {
    const visited = new Map()
    const path = new Set()
    let current = null
    if (result) {
      const nVisited = Math.min(step, result.visited.length)
      for (let i = 0; i < nVisited; i++) visited.set(key(result.visited[i]), i)
      if (nVisited > 0 && step <= result.visited.length) current = key(result.visited[nVisited - 1])
      const nPath = Math.max(0, step - result.visited.length)
      for (let i = 0; i < nPath; i++) path.add(key(result.path[i]))
    }
    return { visited, path, current }
  }, [result, step])

  const done = result !== null && step >= totalFrames
  const found = result !== null && result.path.length > 0

  function clearResult() {
    setResult(null)
    setStep(0)
    setPlaying(false)
  }

  function run() {
    if (!wasm) return
    setResult(wasm.run(grid.flat(), start[0], start[1], end[0], end[1]))
    setStep(0)
    setPlaying(true)
  }

  function skipToEnd() {
    setStep(totalFrames)
    setPlaying(false)
  }

  function applyTool(r, c, isFirst) {
    const cell = [r, c]
    if (tool === 'start' || tool === 'end') {
      if (!isFirst || grid[r][c] === WALL) return
      if (tool === 'start' && !same(cell, end)) setStart(cell)
      if (tool === 'end' && !same(cell, start)) setEnd(cell)
      clearResult()
      return
    }
    if (same(cell, start) || same(cell, end)) return
    if (isFirst) paint.current = grid[r][c] === WALL ? 0 : WALL
    if (paint.current === null || grid[r][c] === paint.current) return
    const value = paint.current
    setGrid((g) => {
      const next = cloneGrid(g)
      next[r][c] = value
      return next
    })
    clearResult()
  }

  function setMap(g, s = start, e = end) {
    setGrid(g)
    setStart(s)
    setEnd(e)
    clearResult()
  }

  // Macierz wynikowa jak w programie konsolowym: 0 / 5 / 3
  const outputMatrix = useMemo(() => {
    const g = cloneGrid(grid)
    if (done && found) for (const [r, c] of result.path) g[r][c] = PATH
    return g
  }, [grid, done, found, result])

  return (
    <div className="app">
      <header>
        <h1>A* na siatce 20×20</h1>
        <p className="sub">
          Algorytm liczy kod C++ skompilowany do WebAssembly: heurystyka euklidesowa, ruchy góra / dół / lewo / prawo,
          koszt ruchu 1.
        </p>
      </header>

      <div className="layout">
        <section className="board-wrap">
          <div className={`board${showValues ? ' values' : ''}`} onPointerLeave={() => setHover(null)}>
            {grid.map((row, r) =>
              row.map((v, c) => {
                const k = key([r, c])
                const isStart = same([r, c], start)
                const isEnd = same([r, c], end)
                let cls = 'cell'
                if (v === WALL) cls += ' wall'
                else if (view.path.has(k)) cls += ' path'
                else if (view.current === k) cls += ' current'
                else if (view.visited.has(k)) cls += ' visited'
                if (isStart) cls += ' start'
                if (isEnd) cls += ' end'
                let label = null
                if (showValues) label = view.path.has(k) ? PATH : v
                else if (isStart) label = 'S'
                else if (isEnd) label = 'C'
                return (
                  <div
                    key={k}
                    className={cls}
                    onPointerDown={(e) => {
                      e.preventDefault()
                      e.currentTarget.releasePointerCapture?.(e.pointerId)
                      applyTool(r, c, true)
                    }}
                    onPointerEnter={() => {
                      setHover([r, c])
                      if (paint.current !== null) applyTool(r, c, false)
                    }}
                  >
                    {label !== null && <span>{label}</span>}
                  </div>
                )
              }),
            )}
          </div>
          <div className="hover-info">
            {hover ? (
              <>
                Pole {fmt(hover)}
                {view.visited.has(key(hover)) && <> · zamknięte jako #{view.visited.get(key(hover)) + 1}</>}
              </>
            ) : (
              'Najedź na pole, żeby zobaczyć współrzędne (wiersz od dołu, kolumna)'
            )}
          </div>
        </section>

        <aside className="panel">
          <div className="group">
            <h2>Edycja mapy</h2>
            <div className="segmented">
              {[
                ['wall', 'Ściany'],
                ['start', 'Start'],
                ['end', 'Cel'],
              ].map(([id, label]) => (
                <button key={id} className={tool === id ? 'active' : ''} onClick={() => setTool(id)}>
                  {label}
                </button>
              ))}
            </div>
            <p className="hint">
              {tool === 'wall'
                ? 'Klikaj lub przeciągaj po siatce, żeby stawiać i usuwać ściany.'
                : `Kliknij wolne pole, żeby ustawić ${tool === 'start' ? 'start' : 'cel'}.`}
            </p>
            <div className="row">
              <button onClick={() => setMap(cloneGrid(DEFAULT_GRID), DEFAULT_START, DEFAULT_END)}>Mapa z zadania</button>
              <button onClick={() => setMap(emptyGrid())}>Pusta</button>
              <button onClick={() => setMap(randomGrid(start, end, 0.28))}>Losowa</button>
            </div>
          </div>

          <div className="group">
            <h2>Uruchomienie</h2>
            <div className="row">
              <button className="primary" onClick={run} disabled={!wasm}>
                {wasm ? 'Uruchom A*' : 'Ładowanie WASM…'}
              </button>
              <button onClick={skipToEnd} disabled={!result || done}>
                Pomiń animację
              </button>
              <button onClick={clearResult} disabled={!result}>
                Wyczyść
              </button>
            </div>
            <label className="slider">
              <span>Szybkość animacji</span>
              <input
                type="range"
                min="0"
                max="120"
                value={120 - delay}
                onChange={(e) => setDelay(120 - Number(e.target.value))}
              />
            </label>
            <label className="check">
              <input type="checkbox" checked={showValues} onChange={(e) => setShowValues(e.target.checked)} />
              Pokaż wartości macierzy (0 / 5 / 3)
            </label>
            {wasmError && <p className="error">Nie udało się załadować WASM: {wasmError}</p>}
          </div>

          <div className="group">
            <h2>Wynik</h2>
            <dl className="stats">
              <dt>Start</dt>
              <dd>{fmt(start)}</dd>
              <dt>Cel</dt>
              <dd>{fmt(end)}</dd>
              <dt>Zamknięte pola</dt>
              <dd>{result ? `${Math.min(step, result.visited.length)} / ${result.visited.length}` : '–'}</dd>
              <dt>Długość trasy</dt>
              <dd>{!done ? '–' : found ? `${result.path.length - 1} ruchów` : 'brak trasy'}</dd>
            </dl>
          </div>

          <div className="group">
            <h2>Legenda</h2>
            <ul className="legend">
              <li><i className="sw start" />Start</li>
              <li><i className="sw end" />Cel</li>
              <li><i className="sw wall" />Przeszkoda (5)</li>
              <li><i className="sw visited" />Odwiedzone (zbiór zamknięty)</li>
              <li><i className="sw path" />Trasa (3)</li>
            </ul>
          </div>
        </aside>
      </div>

      {done && (
        <section className="matrix">
          <h2>Macierz wynikowa</h2>
          <pre>{outputMatrix.map((row) => row.join(' ')).join('\n')}</pre>
        </section>
      )}
    </div>
  )
}
